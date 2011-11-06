/* Eye Of Mate - EOM Plugin
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

#ifndef __EOM_PLUGIN_H__
#define __EOM_PLUGIN_H__

#include "eom-window.h"
#include "eom-debug.h"

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _EomPlugin EomPlugin;
typedef struct _EomPluginClass EomPluginClass;
typedef struct _EomPluginPrivate EomPluginPrivate;

#define EOM_TYPE_PLUGIN            (eom_plugin_get_type())
#define EOM_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_PLUGIN, EomPlugin))
#define EOM_PLUGIN_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_PLUGIN, EomPlugin const))
#define EOM_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_PLUGIN, EomPluginClass))
#define EOM_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_PLUGIN))
#define EOM_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_PLUGIN))
#define EOM_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_PLUGIN, EomPluginClass))

struct _EomPlugin {
	GObject parent;
};

struct _EomPluginClass  {
	GObjectClass parent_class;

	void 		(*activate)		(EomPlugin *plugin,
						 EomWindow *window);

	void 		(*deactivate)		(EomPlugin *plugin,
						 EomWindow *window);

	void 		(*update_ui)		(EomPlugin *plugin,
						 EomWindow *window);

	GtkWidget 	*(*create_configure_dialog)
						(EomPlugin *plugin);

	/* Plugins should not override this, it's handled automatically
 	 * by the EomPluginClass */
	gboolean 	(*is_configurable)
						(EomPlugin *plugin);

	/* Padding for future expansion */
	void		(*_eom_reserved1)	(void);
	void		(*_eom_reserved2)	(void);
	void		(*_eom_reserved3)	(void);
	void		(*_eom_reserved4)	(void);
};

GType 		 eom_plugin_get_type 		(void) G_GNUC_CONST;

void 		 eom_plugin_activate		(EomPlugin *plugin,
						 EomWindow *window);

void 		 eom_plugin_deactivate	        (EomPlugin *plugin,
						 EomWindow *window);

void 		 eom_plugin_update_ui		(EomPlugin *plugin,
						 EomWindow *window);

gboolean	 eom_plugin_is_configurable	(EomPlugin *plugin);

GtkWidget	*eom_plugin_create_configure_dialog
						(EomPlugin *plugin);

/*
 * Utility macro used to register plugins
 *
 * use: EOM_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, CODE)
 */
#define EOM_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, CODE)	\
										\
static GType plugin_name##_type = 0;						\
										\
GType										\
plugin_name##_get_type (void)							\
{										\
	return plugin_name##_type;						\
}										\
										\
static void     plugin_name##_init              (PluginName        *self);	\
static void     plugin_name##_class_init        (PluginName##Class *klass);	\
static gpointer plugin_name##_parent_class = NULL;				\
static void     plugin_name##_class_intern_init (gpointer klass)		\
{										\
	plugin_name##_parent_class = g_type_class_peek_parent (klass);		\
	plugin_name##_class_init ((PluginName##Class *) klass);			\
}										\
										\
G_MODULE_EXPORT GType								\
register_eom_plugin (GTypeModule *module)					\
{										\
	static const GTypeInfo our_info =					\
	{									\
		sizeof (PluginName##Class),					\
		NULL, /* base_init */						\
		NULL, /* base_finalize */					\
		(GClassInitFunc) plugin_name##_class_intern_init,		\
		NULL,								\
		NULL, /* class_data */						\
		sizeof (PluginName),						\
		0, /* n_preallocs */						\
		(GInstanceInitFunc) plugin_name##_init				\
	};									\
										\
	eom_debug_message (DEBUG_PLUGINS, "Registering " #PluginName);	\
										\
	/* Initialise the i18n stuff */						\
	bindtextdomain (GETTEXT_PACKAGE, EOM_LOCALEDIR);			\
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");			\
										\
	plugin_name##_type = g_type_module_register_type (module,		\
					    EOM_TYPE_PLUGIN,			\
					    #PluginName,			\
					    &our_info,				\
					    0);					\
										\
	CODE									\
										\
	return plugin_name##_type;						\
}

/*
 * Utility macro used to register plugins
 *
 * use: EOM_PLUGIN_REGISTER_TYPE(PluginName, plugin_name)
 */
#define EOM_PLUGIN_REGISTER_TYPE(PluginName, plugin_name)			\
	EOM_PLUGIN_REGISTER_TYPE_WITH_CODE(PluginName, plugin_name, ;)

/*
 * Utility macro used to register gobject types in plugins with additional code
 *
 * use: EOM_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, CODE)
 */
#define EOM_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, CODE)	\
										\
static GType g_define_type_id = 0;						\
										\
GType										\
object_name##_get_type (void)							\
{										\
	return g_define_type_id;						\
}										\
										\
static void     object_name##_init              (ObjectName        *self);	\
static void     object_name##_class_init        (ObjectName##Class *klass);	\
static gpointer object_name##_parent_class = NULL;				\
static void     object_name##_class_intern_init (gpointer klass)		\
{										\
	object_name##_parent_class = g_type_class_peek_parent (klass);		\
	object_name##_class_init ((ObjectName##Class *) klass);			\
}										\
										\
GType										\
object_name##_register_type (GTypeModule *module)				\
{										\
	static const GTypeInfo our_info =					\
	{									\
		sizeof (ObjectName##Class),					\
		NULL, /* base_init */						\
		NULL, /* base_finalize */					\
		(GClassInitFunc) object_name##_class_intern_init,		\
		NULL,								\
		NULL, /* class_data */						\
		sizeof (ObjectName),						\
		0, /* n_preallocs */						\
		(GInstanceInitFunc) object_name##_init				\
	};									\
										\
	eom_debug_message (DEBUG_PLUGINS, "Registering " #ObjectName);	\
										\
	g_define_type_id = g_type_module_register_type (module,			\
					   	        PARENT_TYPE,		\
					                #ObjectName,		\
					                &our_info,		\
					                0);			\
										\
	CODE									\
										\
	return g_define_type_id;						\
}

/*
 * Utility macro used to register gobject types in plugins
 *
 * use: EOM_PLUGIN_DEFINE_TYPE(ObjectName, object_name, PARENT_TYPE)
 */
#define EOM_PLUGIN_DEFINE_TYPE(ObjectName, object_name, PARENT_TYPE)		\
	EOM_PLUGIN_DEFINE_TYPE_WITH_CODE(ObjectName, object_name, PARENT_TYPE, ;)

G_END_DECLS

#endif  /* __EOM_PLUGIN_H__ */
