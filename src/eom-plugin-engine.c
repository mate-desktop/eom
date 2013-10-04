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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-plugin-engine.h"
#include "eom-debug.h"
#include "eom-config-keys.h"
#include "eom-util.h"

#include <glib/gi18n.h>
#include <glib.h>
#include <gio/gio.h>
#include <girepository.h>

#define USER_EOM_PLUGINS_LOCATION "plugins/"

struct _EomPluginEnginePrivate {
	GSettings *plugins_settings;
};

G_DEFINE_TYPE_WITH_PRIVATE (EomPluginEngine, eom_plugin_engine, PEAS_TYPE_ENGINE)

static void
eom_plugin_engine_dispose (GObject *object)
{
	EomPluginEngine *engine = EOM_PLUGIN_ENGINE (object);

	if (engine->priv->plugins_settings != NULL)
	{
		g_object_unref (engine->priv->plugins_settings);
		engine->priv->plugins_settings = NULL;
	}

	G_OBJECT_CLASS (eom_plugin_engine_parent_class)->dispose (object);
}

static void
eom_plugin_engine_class_init (EomPluginEngineClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = eom_plugin_engine_dispose;
}

static void
eom_plugin_engine_init (EomPluginEngine *engine)
{
	eom_debug (DEBUG_PLUGINS);

	engine->priv = eom_plugin_engine_get_instance_private (engine);

	engine->priv->plugins_settings = g_settings_new (EOM_CONF_PLUGINS);
}

EomPluginEngine *
eom_plugin_engine_new (void)
{
	EomPluginEngine *engine;
	gchar *user_plugin_path;
	gchar *private_path;
	GError *error = NULL;

	private_path = g_build_filename (LIBDIR, "girepository-1.0", NULL);

	/* This should be moved to libpeas */
	if (g_irepository_require (g_irepository_get_default (),
	                           "Peas", "1.0", 0, &error) == NULL)
	{
		g_warning ("Error loading Peas typelib: %s\n",
		           error->message);
		g_clear_error (&error);
	}

	if (g_irepository_require (g_irepository_get_default (),
	                           "PeasGtk", "1.0", 0, &error) == NULL)
	{
		g_warning ("Error loading PeasGtk typelib: %s\n",
		           error->message);
		g_clear_error (&error);
	}

	if (g_irepository_require_private (g_irepository_get_default (),
	                                   private_path, "Eom", "1.0", 0,
	                                   &error) == NULL)
	{
		g_warning ("Error loading Eom typelib: %s\n",
		           error->message);
		g_clear_error (&error);
	}

	g_free (private_path);

	engine = EOM_PLUGIN_ENGINE (g_object_new (EOM_TYPE_PLUGIN_ENGINE, NULL));

	peas_engine_enable_loader (PEAS_ENGINE (engine), "python3");

	user_plugin_path = g_build_filename (eom_util_dot_dir (),
	                                     USER_EOM_PLUGINS_LOCATION, NULL);

	peas_engine_add_search_path (PEAS_ENGINE (engine),
	                             user_plugin_path, user_plugin_path);

	peas_engine_add_search_path (PEAS_ENGINE (engine),
	                             EOM_PLUGIN_DIR, EOM_PLUGIN_DIR);

	g_settings_bind (engine->priv->plugins_settings,
	                 EOM_CONF_PLUGINS_ACTIVE_PLUGINS,
	                 engine,
	                 "loaded-plugins",
	                 G_SETTINGS_BIND_DEFAULT);

	g_free (user_plugin_path);

	return engine;
}
