#ifndef __EOM_RELOAD_PLUGIN_H__
#define __EOM_RELOAD_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>

#include <eom-window.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define EOM_TYPE_RELOAD_PLUGIN \
	(eom_reload_plugin_get_type())
#define EOM_RELOAD_PLUGIN(o) \
	(G_TYPE_CHECK_INSTANCE_CAST((o), EOM_TYPE_RELOAD_PLUGIN, EomReloadPlugin))
#define EOM_RELOAD_PLUGIN_CLASS(k) \
	(G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_RELOAD_PLUGIN, EomReloadPluginClass))
#define EOM_IS_RELOAD_PLUGIN(o) \
	(G_TYPE_CHECK_INSTANCE_TYPE((o), EOM_TYPE_RELOAD_PLUGIN))
#define EOM_IS_RELOAD_PLUGIN_CLASS(k) \
	(G_TYPE_CHECK_CLASS_TYPE((k), EOM_TYPE_RELOAD_PLUGIN))
#define EOM_RELOAD_PLUGIN_GET_CLASS(o) \
	(G_TYPE_INSTANCE_GET_CLASS((o), EOM_TYPE_RELOAD_PLUGIN, EomReloadPluginClass))

/* Private structure type */
typedef struct _EomReloadPluginPrivate EomReloadPluginPrivate;

/*
 * Main object structure
 */
typedef struct _EomReloadPlugin EomReloadPlugin;

struct _EomReloadPlugin {
	PeasExtensionBase parent_instance;

	EomWindow *window;
	GtkActionGroup *ui_action_group;
	guint ui_id;
};

/*
 * Class definition
 */
typedef struct _EomReloadPluginClass EomReloadPluginClass;

struct _EomReloadPluginClass {
	PeasExtensionBaseClass parent_class;
};

/*
 * Public methods
 */
GType eom_reload_plugin_get_type (void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT void peas_register_types (PeasObjectModule *module);

G_END_DECLS

#endif /* __EOM_RELOAD_PLUGIN_H__ */
