/* Eye Of Mate - Python Plugin
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-python-module.h) by:
 * 	- Raphael Slinckx <raphael@slinckx.net>
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

#ifndef __EOM_PYTHON_PLUGIN_H__
#define __EOM_PYTHON_PLUGIN_H__

#include <Python.h>
#include <glib-object.h>

#include "eom-plugin.h"

G_BEGIN_DECLS

typedef struct _EomPythonPlugin	EomPythonPlugin;
typedef struct _EomPythonPluginClass EomPythonPluginClass;

struct _EomPythonPlugin {
	EomPlugin plugin;

	PyObject *instance;
};

struct _EomPythonPluginClass {
	EomPluginClass parent_class;

	PyObject *type;
};

G_GNUC_INTERNAL
GType eom_python_plugin_get_type (GTypeModule *module, PyObject *type);

G_END_DECLS

#endif
