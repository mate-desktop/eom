/* Eye Of Mate - Application Facade
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on evince code (shell/ev-application.h) by:
 * 	- Martin Kretzschmar <martink@gnome.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "eom-image.h"
#include "eom-session.h"
#include "eom-window.h"
#include "eom-application.h"
#include "eom-application-activatable.h"
#include "eom-application-internal.h"
#include "eom-util.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#define APPLICATION_SERVICE_NAME "org.mate.eom.ApplicationService"

static void eom_application_load_accelerators (void);
static void eom_application_save_accelerators (void);

G_DEFINE_TYPE_WITH_PRIVATE (EomApplication, eom_application, GTK_TYPE_APPLICATION);

static void
eom_application_activate (GApplication *application)
{
	eom_application_open_window (EOM_APPLICATION (application),
				     GDK_CURRENT_TIME,
				     EOM_APPLICATION (application)->priv->flags,
				     NULL);
}

static void
eom_application_open (GApplication *application,
		      GFile       **files,
		      gint          n_files,
		      const gchar  *hint)
{
	GSList *list = NULL;

	while (n_files--)
		list = g_slist_prepend (list, files[n_files]);

	eom_application_open_file_list (EOM_APPLICATION (application),
					list, GDK_CURRENT_TIME,
					EOM_APPLICATION (application)->priv->flags,
					NULL);
}

static void
eom_application_finalize (GObject *object)
{
	EomApplication *application = EOM_APPLICATION (object);
	EomApplicationPrivate *priv = application->priv;

	if (priv->toolbars_model) {
		g_object_unref (priv->toolbars_model);
		priv->toolbars_model = NULL;
		g_free (priv->toolbars_file);
		priv->toolbars_file = NULL;
	}

	g_clear_object (&priv->extensions);

	if (priv->plugin_engine) {
		g_object_unref (priv->plugin_engine);
		priv->plugin_engine = NULL;
	}
	eom_application_save_accelerators ();
}

static void
eom_application_add_platform_data (GApplication *application,
				   GVariantBuilder *builder)
{
	EomApplication *app = EOM_APPLICATION (application);

	G_APPLICATION_CLASS (eom_application_parent_class)->add_platform_data (application,
									       builder);

	if (app->priv->flags) {
		g_variant_builder_add (builder, "{sv}",
				       "eom-application-startup-flags",
				       g_variant_new_byte (app->priv->flags));
	}
}

static void
eom_application_before_emit (GApplication *application,
			     GVariant *platform_data)
{
	GVariantIter iter;
	const gchar *key;
	GVariant *value;

	EOM_APPLICATION (application)->priv->flags = 0;
	g_variant_iter_init (&iter, platform_data);
	while (g_variant_iter_loop (&iter, "{&sv}", &key, &value)) {
		if (strcmp (key, "eom-application-startup-flags") == 0) {
			EOM_APPLICATION (application)->priv->flags = g_variant_get_byte (value);
		}
	}

	G_APPLICATION_CLASS (eom_application_parent_class)->before_emit (application,
									 platform_data);
}

static void
eom_application_class_init (EomApplicationClass *eom_application_class)
{
	GApplicationClass *application_class;
	GObjectClass *object_class;

	application_class = (GApplicationClass *) eom_application_class;
	object_class = (GObjectClass *) eom_application_class;

	object_class->finalize = eom_application_finalize;

	application_class->activate = eom_application_activate;
	application_class->open = eom_application_open;
	application_class->add_platform_data = eom_application_add_platform_data;
	application_class->before_emit = eom_application_before_emit;
}

static void
on_extension_added (PeasExtensionSet *set,
                    PeasPluginInfo   *info,
                    PeasExtension    *exten,
                    EomApplication   *app)
{
	eom_application_activatable_activate (EOM_APPLICATION_ACTIVATABLE (exten));
}

static void
on_extension_removed (PeasExtensionSet *set,
                      PeasPluginInfo   *info,
                      PeasExtension    *exten,
                      EomApplication   *app)
{
	eom_application_activatable_deactivate (EOM_APPLICATION_ACTIVATABLE (exten));
}

static void
eom_application_init (EomApplication *eom_application)
{
	EomApplicationPrivate *priv;
	const gchar *dot_dir = eom_util_dot_dir ();

	eom_session_init (eom_application);

	eom_application->priv = eom_application_get_instance_private (eom_application);
	priv = eom_application->priv;

	priv->toolbars_model = egg_toolbars_model_new ();
	priv->plugin_engine = eom_plugin_engine_new ();
	priv->flags = 0;

	egg_toolbars_model_load_names (priv->toolbars_model,
				       EOM_DATA_DIR "/eom-toolbar.xml");

	if (G_LIKELY (dot_dir != NULL))
		priv->toolbars_file = g_build_filename
			(dot_dir, "eom_toolbar.xml", NULL);

	if (!dot_dir || !egg_toolbars_model_load_toolbars (priv->toolbars_model,
					       priv->toolbars_file)) {

		egg_toolbars_model_load_toolbars (priv->toolbars_model,
						  EOM_DATA_DIR "/eom-toolbar.xml");
	}

	egg_toolbars_model_set_flags (priv->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);

	eom_application_load_accelerators ();

	priv->extensions = peas_extension_set_new (
	                           PEAS_ENGINE (priv->plugin_engine),
	                           EOM_TYPE_APPLICATION_ACTIVATABLE,
	                           "app",  EOM_APPLICATION (eom_application),
	                           NULL);
	peas_extension_set_call (priv->extensions, "activate");
	g_signal_connect (priv->extensions, "extension-added",
	                  G_CALLBACK (on_extension_added), eom_application);
	g_signal_connect (priv->extensions, "extension-removed",
	                  G_CALLBACK (on_extension_removed), eom_application);
}

/**
 * eom_application_get_instance:
 *
 * Returns a singleton instance of #EomApplication currently running.
 * If not running yet, it will create one.
 *
 * Returns: (transfer none): a running #EomApplication.
 **/
EomApplication *
eom_application_get_instance (void)
{
	static EomApplication *instance;

	if (!instance) {
		instance = EOM_APPLICATION (g_object_new (EOM_TYPE_APPLICATION,
							  "application-id", APPLICATION_SERVICE_NAME,
							  "flags", G_APPLICATION_HANDLES_OPEN,
							  NULL));
	}

	return instance;
}

static EomWindow *
eom_application_get_empty_window (EomApplication *application)
{
	EomWindow *empty_window = NULL;
	GList *windows;
	GList *l;

	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	windows = gtk_application_get_windows (GTK_APPLICATION (application));

	for (l = windows; l != NULL; l = l->next) {
		EomWindow *window = EOM_WINDOW (l->data);

		if (eom_window_is_empty (window)) {
			empty_window = window;
			break;
		}
	}

	return empty_window;
}

/**
 * eom_application_open_window:
 * @application: An #EomApplication.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens and presents an empty #EomWindow to the user. If there is
 * an empty window already open, this will be used. Otherwise, a
 * new one will be instantiated.
 *
 * Returns: %FALSE if @application is invalid, %TRUE otherwise
 **/
gboolean
eom_application_open_window (EomApplication  *application,
			     guint32         timestamp,
			     EomStartupFlags flags,
			     GError        **error)
{
	GtkWidget *new_window = NULL;

	new_window = GTK_WIDGET (eom_application_get_empty_window (application));

	if (new_window == NULL) {
		new_window = eom_window_new (flags);
	}

	g_return_val_if_fail (EOM_IS_APPLICATION (application), FALSE);

	gtk_window_present_with_time (GTK_WINDOW (new_window),
				      timestamp);

	return TRUE;
}

static EomWindow *
eom_application_get_file_window (EomApplication *application, GFile *file)
{
	EomWindow *file_window = NULL;
	GList *windows;
	GList *l;

	g_return_val_if_fail (file != NULL, NULL);
	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	windows = gtk_window_list_toplevels ();

	for (l = windows; l != NULL; l = l->next) {
		if (EOM_IS_WINDOW (l->data)) {
			EomWindow *window = EOM_WINDOW (l->data);

			if (!eom_window_is_empty (window)) {
				EomImage *image = eom_window_get_image (window);
				GFile *window_file;

				window_file = eom_image_get_file (image);
				if (g_file_equal (window_file, file)) {
					file_window = window;
					break;
				}
			}
		}
	}

	g_list_free (windows);

	return file_window;
}

static void
eom_application_show_window (EomWindow *window, gpointer user_data)
{
	if (!gtk_widget_get_realized (GTK_WIDGET (window)))
		gtk_widget_realize (GTK_WIDGET (window));

	gtk_window_present_with_time (GTK_WINDOW (window),
				      GPOINTER_TO_UINT (user_data));
}

/**
 * eom_application_open_file_list:
 * @application: An #EomApplication.
 * @file_list: (element-type GFile): A list of #GFile<!-- -->s
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of files in a #EomWindow. If an #EomWindow displaying the first
 * image in the list is already open, this will be used. Otherwise, an empty
 * #EomWindow is used, either already existing or newly created.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_file_list (EomApplication  *application,
				GSList          *file_list,
				guint           timestamp,
				EomStartupFlags flags,
				GError         **error)
{
	EomWindow *new_window = NULL;

	if (file_list != NULL)
		new_window = eom_application_get_file_window (application,
							      (GFile *) file_list->data);

	if (new_window != NULL) {
		gtk_window_present_with_time (GTK_WINDOW (new_window),
					      timestamp);
		return TRUE;
	}

	new_window = eom_application_get_empty_window (application);

	if (new_window == NULL) {
		new_window = EOM_WINDOW (eom_window_new (flags));
	}

	g_signal_connect (new_window,
			  "prepared",
			  G_CALLBACK (eom_application_show_window),
			  GUINT_TO_POINTER (timestamp));

	eom_window_open_file_list (new_window, file_list);

	return TRUE;
}

/**
 * eom_application_open_uri_list:
 * @application: An #EomApplication.
 * @uri_list: (element-type utf8): A list of URIs.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of images, from a list of URIs. See
 * eom_application_open_file_list() for details.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_uri_list (EomApplication  *application,
 			       GSList          *uri_list,
 			       guint           timestamp,
 			       EomStartupFlags flags,
 			       GError         **error)
{
 	GSList *file_list = NULL;

 	g_return_val_if_fail (EOM_IS_APPLICATION (application), FALSE);

 	file_list = eom_util_string_list_to_file_list (uri_list);

 	return eom_application_open_file_list (application,
					       file_list,
					       timestamp,
					       flags,
					       error);
}

/**
 * eom_application_open_uris:
 * @application: an #EomApplication
 * @uris:  A #GList of URI strings.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of images, from a list of URI strings. See
 * eom_application_open_file_list() for details.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_uris (EomApplication  *application,
 			   gchar          **uris,
 			   guint           timestamp,
 			   EomStartupFlags flags,
 			   GError        **error)
{
 	GSList *file_list = NULL;

 	file_list = eom_util_strings_to_file_list (uris);

 	return eom_application_open_file_list (application, file_list, timestamp,
						    flags, error);
}

/**
 * eom_application_get_toolbars_model:
 * @application: An #EomApplication.
 *
 * Retrieves the #EggToolbarsModel for the toolbar in #EomApplication.
 *
 * Returns: (transfer none): An #EggToolbarsModel.
 **/
EggToolbarsModel *
eom_application_get_toolbars_model (EomApplication *application)
{
	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	return application->priv->toolbars_model;
}

/**
 * eom_application_save_toolbars_model:
 * @application: An #EomApplication.
 *
 * Causes the saving of the model of the toolbar in #EomApplication to a file.
 **/
void
eom_application_save_toolbars_model (EomApplication *application)
{
	if (G_LIKELY(application->priv->toolbars_file != NULL))
        	egg_toolbars_model_save_toolbars (application->priv->toolbars_model,
				 	          application->priv->toolbars_file,
						  "1.0");
}

/**
 * eom_application_reset_toolbars_model:
 * @app: an #EomApplication
 *
 * Restores the toolbars model to the defaults.
 **/
void
eom_application_reset_toolbars_model (EomApplication *app)
{
	EomApplicationPrivate *priv;
	g_return_if_fail (EOM_IS_APPLICATION (app));

	priv = app->priv;

	g_object_unref (app->priv->toolbars_model);

	priv->toolbars_model = egg_toolbars_model_new ();

	egg_toolbars_model_load_names (priv->toolbars_model,
				       EOM_DATA_DIR "/eom-toolbar.xml");
	egg_toolbars_model_load_toolbars (priv->toolbars_model,
					  EOM_DATA_DIR "/eom-toolbar.xml");
	egg_toolbars_model_set_flags (priv->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);
}

static void
eom_application_load_accelerators (void)
{
		gchar* accelfile = g_build_filename(g_get_user_config_dir(), "mate", "accels", "eom", NULL);

	/* gtk_accel_map_load does nothing if the file does not exist */
	gtk_accel_map_load (accelfile);
	g_free (accelfile);
}

static void
eom_application_save_accelerators (void)
{
		gchar* accelfile = g_build_filename(g_get_user_config_dir(), "mate", "accels", "eom", NULL);

	gtk_accel_map_save (accelfile);
	g_free (accelfile);
}
