#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-reload-plugin.h"

#include <gmodule.h>
#include <glib/gi18n-lib.h>
#include <libpeas/peas-activatable.h>

#include <eom-debug.h>
#include <eom-window.h>

static void peas_activatable_iface_init (PeasActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (EomReloadPlugin,
                                eom_reload_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (PEAS_TYPE_ACTIVATABLE,
                                                               peas_activatable_iface_init))

enum {
	PROP_0,
	PROP_OBJECT
};

static void
reload_cb (GtkAction *action,
           EomWindow *window)
{
	eom_window_reload_image (window);
}

static const gchar* const ui_definition = "<ui><menubar name=\"MainMenu\">"
	"<menu name=\"ToolsMenu\" action=\"Tools\"><separator/>"
	"<menuitem name=\"EomPluginReload\" action=\"EomPluginRunReload\"/>"
	"<separator/></menu></menubar>"
	"<popup name=\"ViewPopup\"><separator/>"
	"<menuitem action=\"EomPluginRunReload\"/><separator/>"
	"</popup></ui>";

static const GtkActionEntry action_entries[] = {
	{ "EomPluginRunReload", "view-refresh", N_("Reload Image"), "R", N_("Reload current image"), G_CALLBACK (reload_cb) }
};

static void
eom_reload_plugin_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
	EomReloadPlugin *plugin = EOM_RELOAD_PLUGIN (object);

	switch (prop_id)
	{
	case PROP_OBJECT:
		plugin->window = GTK_WIDGET (g_value_dup_object (value));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
eom_reload_plugin_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
	EomReloadPlugin *plugin = EOM_RELOAD_PLUGIN (object);

	switch (prop_id)
	{
	case PROP_OBJECT:
		g_value_set_object (value, plugin->window);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
eom_reload_plugin_init (EomReloadPlugin *plugin)
{
	eom_debug_message (DEBUG_PLUGINS, "EomReloadPlugin initializing");
}

static void
eom_reload_plugin_dispose (GObject *object)
{
	EomReloadPlugin *plugin = EOM_RELOAD_PLUGIN (object);

	eom_debug_message (DEBUG_PLUGINS, "EomReloadPlugin disposing");

	if (plugin->window != NULL) {
		g_object_unref (plugin->window);
		plugin->window = NULL;
	}

	G_OBJECT_CLASS (eom_reload_plugin_parent_class)->dispose (object);
}

static void
eom_reload_plugin_activate (PeasActivatable *activatable)
{
	EomReloadPlugin *plugin = EOM_RELOAD_PLUGIN (activatable);
	GtkUIManager *manager;

	eom_debug (DEBUG_PLUGINS);

	manager = eom_window_get_ui_manager (EOM_WINDOW (plugin->window));

	plugin->ui_action_group = gtk_action_group_new ("EomReloadPluginActions");

	gtk_action_group_set_translation_domain (plugin->ui_action_group, GETTEXT_PACKAGE);

	gtk_action_group_add_actions (plugin->ui_action_group, action_entries,
	                              G_N_ELEMENTS (action_entries), plugin->window);

	gtk_ui_manager_insert_action_group (manager, plugin->ui_action_group, -1);

	plugin->ui_id = gtk_ui_manager_add_ui_from_string (manager, ui_definition, -1, NULL);
	g_warn_if_fail (plugin->ui_id != 0);
}

static void
eom_reload_plugin_deactivate (PeasActivatable *activatable)
{
	EomReloadPlugin *plugin = EOM_RELOAD_PLUGIN (activatable);
	GtkUIManager *manager;

	eom_debug (DEBUG_PLUGINS);

	manager = eom_window_get_ui_manager (EOM_WINDOW (plugin->window));

	gtk_ui_manager_remove_ui (manager, plugin->ui_id);

	gtk_ui_manager_remove_action_group (manager, plugin->ui_action_group);

	gtk_ui_manager_ensure_update (manager);
}

static void
eom_reload_plugin_class_init (EomReloadPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = eom_reload_plugin_dispose;
	object_class->set_property = eom_reload_plugin_set_property;
	object_class->get_property = eom_reload_plugin_get_property;

	g_object_class_override_property (object_class, PROP_OBJECT, "object");
}

static void
eom_reload_plugin_class_finalize (EomReloadPluginClass *klass)
{
	/* dummy function - used by G_DEFINE_DYNAMIC_TYPE_EXTENDED */
}

static void
peas_activatable_iface_init (PeasActivatableInterface *iface)
{
	iface->activate = eom_reload_plugin_activate;
	iface->deactivate = eom_reload_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
	eom_reload_plugin_register_type (G_TYPE_MODULE (module));
	peas_object_module_register_extension_type (module,
	                                            PEAS_TYPE_ACTIVATABLE,
	                                            EOM_TYPE_RELOAD_PLUGIN);
}
