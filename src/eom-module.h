/* Eye Of Mate - Main Window
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

#ifndef EOM_MODULE_H
#define EOM_MODULE_H

#include <glib-object.h>
#include <gmodule.h>

G_BEGIN_DECLS

typedef struct _EomModule EomModule;
typedef struct _EomModuleClass EomModuleClass;
typedef struct _EomModulePrivate EomModulePrivate;

#define EOM_TYPE_MODULE            (eom_module_get_type ())
#define EOM_MODULE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_MODULE, EomModule))
#define EOM_MODULE_CLASS(klass)	   (G_TYPE_CHECK_CLASS_CAST ((klass),  EOM_TYPE_MODULE, EomModuleClass))
#define EOM_IS_MODULE(obj)	   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_MODULE))
#define EOM_IS_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj),    EOM_TYPE_MODULE))
#define EOM_MODULE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),   EOM_TYPE_MODULE, EomModuleClass))

struct _EomModule {
	GTypeModule parent_instance;

	GModule *library;
	gchar   *path;
	GType    type;
};

struct _EomModuleClass {
	GTypeModuleClass parent_class;
};

G_GNUC_INTERNAL
GType		 eom_module_get_type	(void) G_GNUC_CONST;

G_GNUC_INTERNAL
EomModule	*eom_module_new		(const gchar *path);

G_GNUC_INTERNAL
const gchar	*eom_module_get_path	(EomModule *module);

G_GNUC_INTERNAL
GObject		*eom_module_new_object	(EomModule *module);

G_END_DECLS

#endif
