/* Eye Of Mate - EOM Module
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-module.c) by:
 * 	- Paolo Maggi <paolo@gnome.org>
 *      - Marco Pesenti Gritti <marco@gnome.org>
 *      - Christian Persch <chpe@gnome.org>
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
#include "config.h"
#endif

#include "eom-module.h"
#include "eom-debug.h"

#include <gmodule.h>

#define EOM_MODULE_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EOM_TYPE_MODULE, EomModulePrivate))

G_DEFINE_TYPE (EomModule, eom_module, G_TYPE_TYPE_MODULE);

typedef GType (*EomModuleRegisterFunc) (GTypeModule *);

static gboolean
eom_module_load (GTypeModule *gmodule)
{
	EomModule *module = EOM_MODULE (gmodule);
	EomModuleRegisterFunc register_func;

	eom_debug_message (DEBUG_PLUGINS, "Loading %s", module->path);

	module->library = g_module_open (module->path, 0);

	if (module->library == NULL) {
		g_warning ("%s", g_module_error());

		return FALSE;
	}

	/* Extract symbols from the lib */
	if (!g_module_symbol (module->library,
			      "register_eom_plugin",
			      (void *) &register_func)) {
		g_warning ("%s", g_module_error());
		g_module_close (module->library);

		return FALSE;
	}

	/* Symbol can still be NULL even though g_module_symbol
	 * returned TRUE */
	if (register_func == NULL) {
		g_warning ("Symbol 'register_eom_plugin' should not be NULL");
		g_module_close (module->library);

		return FALSE;
	}

	module->type = register_func (gmodule);

	if (module->type == 0) {
		g_warning ("Invalid eom plugin contained by module %s", module->path);
		return FALSE;
	}

	return TRUE;
}

static void
eom_module_unload (GTypeModule *gmodule)
{
	EomModule *module = EOM_MODULE (gmodule);

	eom_debug_message (DEBUG_PLUGINS, "Unloading %s", module->path);

	g_module_close (module->library);

	module->library = NULL;
	module->type = 0;
}

const gchar *
eom_module_get_path (EomModule *module)
{
	g_return_val_if_fail (EOM_IS_MODULE (module), NULL);

	return module->path;
}

GObject *
eom_module_new_object (EomModule *module)
{
	eom_debug_message (DEBUG_PLUGINS, "Creating object of type %s", g_type_name (module->type));

	if (module->type == 0) {
		return NULL;
	}

	return g_object_new (module->type, NULL);
}

static void
eom_module_init (EomModule *module)
{
	eom_debug_message (DEBUG_PLUGINS, "EomModule %p initialising", module);
}

static void
eom_module_finalize (GObject *object)
{
	EomModule *module = EOM_MODULE (object);

	eom_debug_message (DEBUG_PLUGINS, "EomModule %p finalising", module);

	g_free (module->path);

	G_OBJECT_CLASS (eom_module_parent_class)->finalize (object);
}

static void
eom_module_class_init (EomModuleClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GTypeModuleClass *module_class = G_TYPE_MODULE_CLASS (class);

	object_class->finalize = eom_module_finalize;

	module_class->load = eom_module_load;
	module_class->unload = eom_module_unload;
}

EomModule *
eom_module_new (const gchar *path)
{
	EomModule *module;

	if (path == NULL || path[0] == '\0') {
		return NULL;
	}

	module = g_object_new (EOM_TYPE_MODULE, NULL);

	g_type_module_set_name (G_TYPE_MODULE (module), path);

	module->path = g_strdup (path);

	return module;
}
