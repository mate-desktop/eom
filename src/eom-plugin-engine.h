/* Eye Of Mate - EOM Plugin Engine
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-plugins-engine.h) by:
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

#ifndef __EOM_PLUGIN_ENGINE_H__
#define __EOM_PLUGIN_ENGINE_H__

#include <glib.h>
#include <libpeas/peas-engine.h>

G_BEGIN_DECLS

typedef struct _EomPluginEngine EomPluginEngine;
typedef struct _EomPluginEngineClass EomPluginEngineClass;
typedef struct _EomPluginEnginePrivate EomPluginEnginePrivate;

#define EOM_TYPE_PLUGIN_ENGINE            eom_plugin_engine_get_type()
#define EOM_PLUGIN_ENGINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_PLUGIN_ENGINE, EomPluginEngine))
#define EOM_PLUGIN_ENGINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EOM_TYPE_PLUGIN_ENGINE, EomPluginEngineClass))
#define EOM_IS_PLUGIN_ENGINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_PLUGIN_ENGINE))
#define EOM_IS_PLUGIN_ENGINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_PLUGIN_ENGINE))
#define EOM_PLUGIN_ENGINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EOM_TYPE_PLUGIN_ENGINE, EomPluginEngineClass))

struct _EomPluginEngine {
	PeasEngine parent;
	EomPluginEnginePrivate *priv;
};

struct _EomPluginEngineClass {
	PeasEngineClass parent_class;
};

GType eom_plugin_engine_get_type (void) G_GNUC_CONST;

EomPluginEngine* eom_plugin_engine_new (void);

G_END_DECLS

#endif  /* __EOM_PLUGIN_ENGINE_H__ */
