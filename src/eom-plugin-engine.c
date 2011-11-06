/* Eye Of Mate - EOM Plugin Manager
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-module.c) by:
 * 	- Paolo Maggi <paolo@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "eom-plugin-engine.h"
#include "eom-plugin.h"
#include "eom-module.h"
#include "eom-debug.h"
#include "eom-application.h"
#include "eom-config-keys.h"
#include "eom-util.h"

#include <glib/gi18n.h>
#include <glib.h>
#include <mateconf/mateconf-client.h>

#ifdef ENABLE_PYTHON
#include "eom-python-module.h"
#endif

#define USER_EOM_PLUGINS_LOCATION "plugins/"

#define EOM_PLUGINS_ENGINE_BASE_KEY "/apps/eom/plugins"

#define PLUGIN_EXT	".eom-plugin"

typedef enum {
	EOM_PLUGIN_LOADER_C,
	EOM_PLUGIN_LOADER_PY,
} EomPluginLoader;

struct _EomPluginInfo
{
	gchar             *file;

	gchar             *location;
	EomPluginLoader    loader;
	GTypeModule       *module;

	gchar             *name;
	gchar             *desc;
	gchar             *icon_name;
	gchar            **authors;
	gchar             *copyright;
	gchar             *website;

	EomPlugin         *plugin;

	gint               active : 1;

	/* A plugin is unavailable if it is not possible to activate it
	   due to an error loading the plugin module (e.g. for Python plugins
	   when the interpreter has not been correctly initializated) */
	gint               available : 1;
};

static void	eom_plugin_engine_active_plugins_changed (MateConfClient *client,
							  guint cnxn_id,
							  MateConfEntry *entry,
							  gpointer user_data);

static GList *eom_plugins_list = NULL;

static MateConfClient *eom_plugin_engine_mateconf_client = NULL;

static GSList *active_plugins = NULL;

static void
eom_plugin_info_free (EomPluginInfo *info)
{
	if (info->plugin != NULL) {
	       	eom_debug_message (DEBUG_PLUGINS, "Unref plugin %s", info->name);

		g_object_unref (info->plugin);
	}

	g_free (info->file);
	g_free (info->location);
	g_free (info->name);
	g_free (info->desc);
	g_free (info->icon_name);
	g_free (info->website);
	g_free (info->copyright);
	g_strfreev (info->authors);

	g_free (info);
}

static EomPluginInfo *
eom_plugin_engine_load (const gchar *file)
{
	EomPluginInfo *info;
	GKeyFile *plugin_file = NULL;
	gchar *str;

	g_return_val_if_fail (file != NULL, NULL);

	eom_debug_message (DEBUG_PLUGINS, "Loading plugin: %s", file);

	info = g_new0 (EomPluginInfo, 1);
	info->file = g_strdup (file);

	plugin_file = g_key_file_new ();

	if (!g_key_file_load_from_file (plugin_file, file, G_KEY_FILE_NONE, NULL)) {
		g_warning ("Bad plugin file: %s", file);

		goto error;
	}

	if (!g_key_file_has_key (plugin_file,
			   	 "Eom Plugin",
				 "IAge",
				 NULL))	{
		eom_debug_message (DEBUG_PLUGINS,
				   "IAge key does not exist in file: %s", file);

		goto error;
	}

	/* Check IAge=2 */
	if (g_key_file_get_integer (plugin_file,
				    "Eom Plugin",
				    "IAge",
				    NULL) != 2) {
		eom_debug_message (DEBUG_PLUGINS,
				   "Wrong IAge in file: %s", file);

		goto error;
	}

	/* Get Location */
	str = g_key_file_get_string (plugin_file,
				     "Eom Plugin",
				     "Module",
				     NULL);

	if ((str != NULL) && (*str != '\0')) {
		info->location = str;
	} else {
		g_warning ("Could not find 'Module' in %s", file);

		goto error;
	}

	/* Get the loader for this plugin */
	str = g_key_file_get_string (plugin_file,
				     "Eom Plugin",
				     "Loader",
				     NULL);

	if (str && strcmp(str, "python") == 0) {
		info->loader = EOM_PLUGIN_LOADER_PY;

#ifndef ENABLE_PYTHON
		g_warning ("Cannot load Python plugin '%s' since eom was not "
			   "compiled with Python support.", file);

		goto error;
#endif

	} else {
		info->loader = EOM_PLUGIN_LOADER_C;
	}

	g_free (str);

	/* Get Name */
	str = g_key_file_get_locale_string (plugin_file,
					    "Eom Plugin",
					    "Name",
					    NULL, NULL);
	if (str) {
		info->name = str;
	} else {
		g_warning ("Could not find 'Name' in %s", file);

		goto error;
	}

	/* Get Description */
	str = g_key_file_get_locale_string (plugin_file,
					    "Eom Plugin",
					    "Description",
					    NULL, NULL);
	if (str) {
		info->desc = str;
	} else {
		eom_debug_message (DEBUG_PLUGINS, "Could not find 'Description' in %s", file);
 	}

	/* Get Icon */
	str = g_key_file_get_locale_string (plugin_file,
					    "Eom Plugin",
					    "Icon",
					    NULL, NULL);
	if (str) {
		info->icon_name = str;
	} else {
		eom_debug_message (DEBUG_PLUGINS, "Could not find 'Icon' in %s, "
						  "using 'eom-plugin'", file);
	}

	/* Get Authors */
	info->authors = g_key_file_get_string_list (plugin_file,
						    "Eom Plugin",
						    "Authors",
						    NULL,
						    NULL);

	if (info->authors == NULL)
		eom_debug_message (DEBUG_PLUGINS, "Could not find 'Authors' in %s", file);


	/* Get Copyright */
	str = g_key_file_get_string (plugin_file,
				     "Eom Plugin",
				     "Copyright",
				     NULL);
	if (str) {
		info->copyright = str;
	} else {
		eom_debug_message (DEBUG_PLUGINS, "Could not find 'Copyright' in %s", file);
	}

	/* Get Website */
	str = g_key_file_get_string (plugin_file,
				     "Eom Plugin",
				     "Website",
				     NULL);
	if (str) {
		info->website = str;
	} else {
		eom_debug_message (DEBUG_PLUGINS, "Could not find 'Website' in %s", file);
	}

	g_key_file_free (plugin_file);

	/* If we know nothing about the availability of the plugin,
	   set it as available */
	info->available = TRUE;

	return info;

error:
	g_free (info->file);
	g_free (info->location);
	g_free (info->name);
	g_free (info);
	g_key_file_free (plugin_file);

	return NULL;
}

static gint
compare_plugin_info (EomPluginInfo *info1,
		     EomPluginInfo *info2)
{
	return strcmp (info1->location, info2->location);
}

static void
eom_plugin_engine_load_dir (const gchar *dir)
{
	GError *error = NULL;
	GDir *d;
	const gchar *dirent;

	if (!g_file_test (dir, G_FILE_TEST_IS_DIR)) {
		return;
	}

	g_return_if_fail (eom_plugin_engine_mateconf_client != NULL);

	eom_debug_message (DEBUG_PLUGINS, "DIR: %s", dir);

	d = g_dir_open (dir, 0, &error);

	if (!d)	{
		g_warning ("%s", error->message);
		g_error_free (error);

		return;
	}

	while ((dirent = g_dir_read_name (d))) {
		if (g_str_has_suffix (dirent, PLUGIN_EXT)) {
			gchar *plugin_file;
			EomPluginInfo *info;

			plugin_file = g_build_filename (dir, dirent, NULL);
			info = eom_plugin_engine_load (plugin_file);
			g_free (plugin_file);

			if (info == NULL)
				continue;

			/* If a plugin with this name has already been loaded
			 * drop this one (user plugins override system plugins) */
			if (g_list_find_custom (eom_plugins_list,
						info,
						(GCompareFunc)compare_plugin_info) != NULL) {
				g_warning ("Two or more plugins named '%s'. "
					   "Only the first will be considered.\n",
					   info->location);

				eom_plugin_info_free (info);

				continue;
			}

			/* Actually, the plugin will be activated when reactivate_all
			 * will be called for the first time. */
			info->active = (g_slist_find_custom (active_plugins,
							     info->location,
							     (GCompareFunc)strcmp) != NULL);

			eom_plugins_list = g_list_prepend (eom_plugins_list, info);

			eom_debug_message (DEBUG_PLUGINS, "Plugin %s loaded", info->name);
		}
	}

	eom_plugins_list = g_list_reverse (eom_plugins_list);

	g_dir_close (d);
}

static void
eom_plugin_engine_load_all (void)
{
	gchar *pdir;

	pdir = g_build_filename (eom_util_dot_dir (),
                                 USER_EOM_PLUGINS_LOCATION, NULL);

	/* Load user's plugins */
	eom_plugin_engine_load_dir (pdir);

	g_free (pdir);

	/* Load system plugins */
	eom_plugin_engine_load_dir (EOM_PLUGIN_DIR "/");
}

gboolean
eom_plugin_engine_init (void)
{
	eom_debug (DEBUG_PLUGINS);

	g_return_val_if_fail (eom_plugins_list == NULL, FALSE);

	if (!g_module_supported ()) {
		g_warning ("eom is not able to initialize the plugins engine.");

		return FALSE;
	}

	eom_plugin_engine_mateconf_client = mateconf_client_get_default ();

	g_return_val_if_fail (eom_plugin_engine_mateconf_client != NULL, FALSE);

	mateconf_client_add_dir (eom_plugin_engine_mateconf_client,
			      EOM_PLUGINS_ENGINE_BASE_KEY,
			      MATECONF_CLIENT_PRELOAD_ONELEVEL,
			      NULL);

	mateconf_client_notify_add (eom_plugin_engine_mateconf_client,
				 EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
				 eom_plugin_engine_active_plugins_changed,
				 NULL, NULL, NULL);

	active_plugins = mateconf_client_get_list (eom_plugin_engine_mateconf_client,
						EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
						MATECONF_VALUE_STRING,
						NULL);

	eom_plugin_engine_load_all ();

	return TRUE;
}

void
eom_plugin_engine_garbage_collect (void)
{
#ifdef ENABLE_PYTHON
	eom_python_garbage_collect ();
#endif
}

void
eom_plugin_engine_shutdown (void)
{
	GList *pl;

	eom_debug (DEBUG_PLUGINS);

#ifdef ENABLE_PYTHON
	/* Note: that this may cause finalization of objects (typically
	 * the EomWindow) by running the garbage collector. Since some
	 * of the plugin may have installed callbacks upon object
	 * finalization (typically they need to free the WindowData)
	 * it must run before we get rid of the plugins.
	 */
	eom_python_shutdown ();
#endif

	g_return_if_fail (eom_plugin_engine_mateconf_client != NULL);

	for (pl = eom_plugins_list; pl; pl = pl->next) {
		EomPluginInfo *info = (EomPluginInfo*) pl->data;

		eom_plugin_info_free (info);
	}

	g_slist_foreach (active_plugins, (GFunc)g_free, NULL);
	g_slist_free (active_plugins);

	active_plugins = NULL;

	g_list_free (eom_plugins_list);
	eom_plugins_list = NULL;

	g_object_unref (eom_plugin_engine_mateconf_client);
	eom_plugin_engine_mateconf_client = NULL;
}

const GList *
eom_plugin_engine_get_plugins_list (void)
{
	eom_debug (DEBUG_PLUGINS);

	return eom_plugins_list;
}

static gboolean
load_plugin_module (EomPluginInfo *info)
{
	gchar *path;
	gchar *dirname;

	eom_debug (DEBUG_PLUGINS);

	g_return_val_if_fail (info != NULL, FALSE);
	g_return_val_if_fail (info->file != NULL, FALSE);
	g_return_val_if_fail (info->location != NULL, FALSE);
	g_return_val_if_fail (info->plugin == NULL, FALSE);
	g_return_val_if_fail (info->available, FALSE);

	switch (info->loader) {
	case EOM_PLUGIN_LOADER_C:
		dirname = g_path_get_dirname (info->file);
		g_return_val_if_fail (dirname != NULL, FALSE);

		path = g_module_build_path (dirname, info->location);

		g_free (dirname);

		g_return_val_if_fail (path != NULL, FALSE);

		info->module = G_TYPE_MODULE (eom_module_new (path));

		g_free (path);

		break;

#ifdef ENABLE_PYTHON
	case EOM_PLUGIN_LOADER_PY:
	{
		gchar *dir;

		if (!eom_python_init ()) {
			/* Mark plugin as unavailable and fails */
			info->available = FALSE;

			g_warning ("Cannot load Python plugin '%s' since eom "
			           "was not able to initialize the Python interpreter.",
			           info->name);

			return FALSE;
		}

		dir = g_path_get_dirname (info->file);

		g_return_val_if_fail ((info->location != NULL) &&
		                      (info->location[0] != '\0'),
		                      FALSE);

		info->module = G_TYPE_MODULE (
				eom_python_module_new (dir, info->location));

		g_free (dir);

		break;
	}
#endif
	default:
		g_return_val_if_reached (FALSE);
	}

	if (!g_type_module_use (info->module)) {
		switch (info->loader) {
		case EOM_PLUGIN_LOADER_C:
			g_warning ("Cannot load plugin '%s' since file '%s' cannot be read.",
				   info->name,
				   eom_module_get_path (EOM_MODULE (info->module)));
			break;

		case EOM_PLUGIN_LOADER_PY:
			g_warning ("Cannot load Python plugin '%s' since file '%s' cannot be read.",
				   info->name,
				   info->location);
			break;

		default:
			g_return_val_if_reached (FALSE);
		}

		g_object_unref (G_OBJECT (info->module));

		info->module = NULL;

		/* Mark plugin as unavailable and fails */
		info->available = FALSE;

		return FALSE;
	}

	switch (info->loader) {
	case EOM_PLUGIN_LOADER_C:
		info->plugin =
			EOM_PLUGIN (eom_module_new_object (EOM_MODULE (info->module)));
		break;

#ifdef ENABLE_PYTHON
	case EOM_PLUGIN_LOADER_PY:
		info->plugin =
			EOM_PLUGIN (eom_python_module_new_object (EOM_PYTHON_MODULE (info->module)));
		break;
#endif

	default:
		g_return_val_if_reached (FALSE);
	}

	g_type_module_unuse (info->module);

	eom_debug_message (DEBUG_PLUGINS, "End");

	return TRUE;
}

static gboolean
eom_plugin_engine_activate_plugin_real (EomPluginInfo *info)
{
	gboolean res = TRUE;

	/* Plugin is not available, don't try to activate/load it */
	if (!info->available) {
		return FALSE;
	}

	if (info->plugin == NULL)
		res = load_plugin_module (info);

	if (res) {
		const GList *wins = eom_application_get_windows (EOM_APP);

		while (wins != NULL) {
			eom_plugin_activate (info->plugin,
					     EOM_WINDOW (wins->data));

			wins = g_list_next (wins);
		}
	} else {
		g_warning ("Error activating plugin '%s'", info->name);
	}

	return res;
}

gboolean
eom_plugin_engine_activate_plugin (EomPluginInfo *info)
{
	eom_debug (DEBUG_PLUGINS);

	g_return_val_if_fail (info != NULL, FALSE);

	if (!info->available)
		return FALSE;

	if (info->active)
		return TRUE;

	if (eom_plugin_engine_activate_plugin_real (info)) {
		gboolean res;
		GSList *list;

		/* Update plugin state */
		info->active = TRUE;

		list = active_plugins;

		while (list != NULL) {
			if (strcmp (info->location, (gchar *)list->data) == 0) {
				g_warning ("Plugin '%s' is already active.", info->name);

				return TRUE;
			}

			list = g_slist_next (list);
		}

		active_plugins = g_slist_insert_sorted (active_plugins,
						        g_strdup (info->location),
						        (GCompareFunc)strcmp);

		res = mateconf_client_set_list (eom_plugin_engine_mateconf_client,
		    			     EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
					     MATECONF_VALUE_STRING,
					     active_plugins,
					     NULL);

		if (!res)
			g_warning ("Error saving the list of active plugins.");

		return TRUE;
	}

	return FALSE;
}

static void
eom_plugin_engine_deactivate_plugin_real (EomPluginInfo *info)
{
	const GList *wins = eom_application_get_windows (EOM_APP);

	while (wins != NULL) {
		eom_plugin_deactivate (info->plugin,
				       EOM_WINDOW (wins->data));

		wins = g_list_next (wins);
	}
}

gboolean
eom_plugin_engine_deactivate_plugin (EomPluginInfo *info)
{
	gboolean res;
	GSList *list;

	eom_debug (DEBUG_PLUGINS);

	g_return_val_if_fail (info != NULL, FALSE);

	if (!info->active || !info->available)
		return TRUE;

	eom_plugin_engine_deactivate_plugin_real (info);

	/* Update plugin state */
	info->active = FALSE;

	list = active_plugins;
	res = (list == NULL);

	while (list != NULL) {
		if (strcmp (info->location, (gchar *)list->data) == 0) {
			g_free (list->data);
			active_plugins = g_slist_delete_link (active_plugins, list);
			list = NULL;
			res = TRUE;
		} else {
			list = g_slist_next (list);
		}
	}

	if (!res) {
		g_warning ("Plugin '%s' is already deactivated.", info->name);

		return TRUE;
	}

	res = mateconf_client_set_list (eom_plugin_engine_mateconf_client,
	    			     EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
				     MATECONF_VALUE_STRING,
				     active_plugins,
				     NULL);

	if (!res)
		g_warning ("Error saving the list of active plugins.");

	return TRUE;
}

gboolean
eom_plugin_engine_plugin_is_active (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, FALSE);

	return (info->available && info->active);
}

gboolean
eom_plugin_engine_plugin_is_available (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, FALSE);

	return (info->available != FALSE);
}

static void
reactivate_all (EomWindow *window)
{
	GList *pl;

	eom_debug (DEBUG_PLUGINS);

	for (pl = eom_plugins_list; pl; pl = pl->next) {
		gboolean res = TRUE;

		EomPluginInfo *info = (EomPluginInfo*)pl->data;

		/* If plugin is not available, don't try to activate/load it */
		if (info->available && info->active) {
			if (info->plugin == NULL)
				res = load_plugin_module (info);

			if (res)
				eom_plugin_activate (info->plugin,
						     window);
		}
	}

	eom_debug_message (DEBUG_PLUGINS, "End");
}

void
eom_plugin_engine_update_plugins_ui (EomWindow *window,
				     gboolean   new_window)
{
	GList *pl;

	eom_debug (DEBUG_PLUGINS);

	g_return_if_fail (EOM_IS_WINDOW (window));

	if (new_window)
		reactivate_all (window);

	/* Updated ui of all the plugins that implement update_ui */
	for (pl = eom_plugins_list; pl; pl = pl->next) {
		EomPluginInfo *info = (EomPluginInfo*)pl->data;

		if (!info->available || !info->active)
			continue;

	       	eom_debug_message (DEBUG_PLUGINS, "Updating UI of %s", info->name);

		eom_plugin_update_ui (info->plugin, window);
	}
}

gboolean
eom_plugin_engine_plugin_is_configurable (EomPluginInfo *info) {
	eom_debug (DEBUG_PLUGINS);

	g_return_val_if_fail (info != NULL, FALSE);

	if ((info->plugin == NULL) || !info->active || !info->available)
		return FALSE;

	return eom_plugin_is_configurable (info->plugin);
}

void
eom_plugin_engine_configure_plugin (EomPluginInfo *info,
				    GtkWindow     *parent)
{
	GtkWidget *conf_dlg;

	GtkWindowGroup *wg;

	eom_debug (DEBUG_PLUGINS);

	g_return_if_fail (info != NULL);

	conf_dlg = eom_plugin_create_configure_dialog (info->plugin);

	g_return_if_fail (conf_dlg != NULL);

	gtk_window_set_transient_for (GTK_WINDOW (conf_dlg),
				      parent);

	// Will return a default group if no group is set
	wg = gtk_window_get_group (parent);

	// For now assign a dedicated window group if it is 
	// the default one until we know if this is really needed
	if (wg == gtk_window_get_group (NULL)) {
		wg = gtk_window_group_new ();
		gtk_window_group_add_window (wg, parent);
	}

	gtk_window_group_add_window (wg,
				     GTK_WINDOW (conf_dlg));

	gtk_window_set_modal (GTK_WINDOW (conf_dlg), TRUE);

	gtk_widget_show (conf_dlg);
}

static void
eom_plugin_engine_active_plugins_changed (MateConfClient *client,
					  guint cnxn_id,
					  MateConfEntry *entry,
					  gpointer user_data)
{
	GList *pl;
	gboolean to_activate;

	eom_debug (DEBUG_PLUGINS);

	g_return_if_fail (entry->key != NULL);
	g_return_if_fail (entry->value != NULL);

	if (!((entry->value->type == MATECONF_VALUE_LIST) &&
	      (mateconf_value_get_list_type (entry->value) == MATECONF_VALUE_STRING))) {
		g_warning ("The mateconf key '%s' may be corrupted.", EOM_CONF_PLUGINS_ACTIVE_PLUGINS);
		return;
	}

	active_plugins = mateconf_client_get_list (eom_plugin_engine_mateconf_client,
						EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
						MATECONF_VALUE_STRING,
						NULL);

	for (pl = eom_plugins_list; pl; pl = pl->next) {
		EomPluginInfo *info = (EomPluginInfo*)pl->data;

		if (!info->available)
			continue;

		to_activate = (g_slist_find_custom (active_plugins,
						    info->location,
						    (GCompareFunc)strcmp) != NULL);

		if (!info->active && to_activate) {
			/* Activate plugin */
			if (eom_plugin_engine_activate_plugin_real (info))
				/* Update plugin state */
				info->active = TRUE;
		} else {
			if (info->active && !to_activate) {
				eom_plugin_engine_deactivate_plugin_real (info);

				/* Update plugin state */
				info->active = FALSE;
			}
		}
	}
}

const gchar *
eom_plugin_engine_get_plugin_name (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->name;
}

const gchar *
eom_plugin_engine_get_plugin_description (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->desc;
}

const gchar *
eom_plugin_engine_get_plugin_icon_name (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	/* Use the eom-plugin icon as a default if the plugin does not
	   have its own */
	if (info->icon_name != NULL &&
	    gtk_icon_theme_has_icon (gtk_icon_theme_get_default (),
	    			     info->icon_name))
		return info->icon_name;
	else
		return "eom-plugin";
}

const gchar **
eom_plugin_engine_get_plugin_authors (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, (const gchar **)NULL);

	return (const gchar **) info->authors;
}

const gchar *
eom_plugin_engine_get_plugin_website (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->website;
}

const gchar *
eom_plugin_engine_get_plugin_copyright (EomPluginInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->copyright;
}
