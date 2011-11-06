/* Eye Of Mate - Python Module
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

#ifndef __EOM_PYTHON_MODULE_H__
#define __EOM_PYTHON_MODULE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define EOM_TYPE_PYTHON_MODULE		  (eom_python_module_get_type ())
#define EOM_PYTHON_MODULE(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_PYTHON_MODULE, EomPythonModule))
#define EOM_PYTHON_MODULE_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), EOM_TYPE_PYTHON_MODULE, EomPythonModuleClass))
#define EOM_IS_PYTHON_MODULE(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_PYTHON_MODULE))
#define EOM_IS_PYTHON_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), EOM_TYPE_PYTHON_MODULE))
#define EOM_PYTHON_MODULE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), EOM_TYPE_PYTHON_MODULE, EomPythonModuleClass))

typedef struct _EomPythonModule	EomPythonModule;
typedef struct _EomPythonModuleClass EomPythonModuleClass;
typedef struct _EomPythonModulePrivate EomPythonModulePrivate;

struct _EomPythonModuleClass {
	GTypeModuleClass parent_class;
};

struct _EomPythonModule {
	GTypeModule parent_instance;
};

G_GNUC_INTERNAL
GType			 eom_python_module_get_type		(void) G_GNUC_CONST;

G_GNUC_INTERNAL
EomPythonModule		*eom_python_module_new			(const gchar* path,
								 const gchar *module);

G_GNUC_INTERNAL
GObject			*eom_python_module_new_object		(EomPythonModule *module);

G_GNUC_INTERNAL
gboolean		eom_python_init				(void);

G_GNUC_INTERNAL
void			eom_python_shutdown			(void);

G_GNUC_INTERNAL
void			eom_python_garbage_collect		(void);

G_END_DECLS

#endif /* __EOM_PYTHON_MODULE_H__ */
