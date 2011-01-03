#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-fullscreen-plugin.h"

#include <gmodule.h>
#include <glib/gi18n-lib.h>
#include <libpeas/peas-activatable.h>

#include <eom-debug.h>
#include <eom-window.h>
#include <eom-window-activatable.h>

static void eom_window_activatable_iface_init (EomWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (EomFullscreenPlugin,
                                eom_fullscreen_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (EOM_TYPE_WINDOW_ACTIVATABLE,
                                                               eom_window_activatable_iface_init))

enum {
	PROP_0,
	PROP_WINDOW
};

static gboolean
on_button_press (GtkWidget      *button,
                 GdkEventButton *event,
                 EomWindow      *window)
{
	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS)
	{
		EomWindowMode mode = eom_window_get_mode (window);

		if (mode == EOM_WINDOW_MODE_SLIDESHOW || mode == EOM_WINDOW_MODE_FULLSCREEN)
		{
			eom_window_set_mode (window, EOM_WINDOW_MODE_NORMAL);
		}
		else if (mode == EOM_WINDOW_MODE_NORMAL)
		{
			eom_window_set_mode (window, EOM_WINDOW_MODE_FULLSCREEN);
		}

		return TRUE;
	}

	return FALSE;
}

static void
eom_fullscreen_plugin_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
	EomFullscreenPlugin *plugin = EOM_FULLSCREEN_PLUGIN (object);

	switch (prop_id)
	{
	case PROP_WINDOW:
		plugin->window = EOM_WINDOW (g_value_dup_object (value));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
eom_fullscreen_plugin_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
	EomFullscreenPlugin *plugin = EOM_FULLSCREEN_PLUGIN (object);

	switch (prop_id)
	{
	case PROP_WINDOW:
		g_value_set_object (value, plugin->window);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
eom_fullscreen_plugin_init (EomFullscreenPlugin *plugin)
{
	eom_debug_message (DEBUG_PLUGINS, "EomFullscreenPlugin initializing");
}

static void
eom_fullscreen_plugin_dispose (GObject *object)
{
	EomFullscreenPlugin *plugin = EOM_FULLSCREEN_PLUGIN (object);

	eom_debug_message (DEBUG_PLUGINS, "EomFullscreenPlugin disposing");

	if (plugin->window != NULL) {
		g_object_unref (plugin->window);
		plugin->window = NULL;
	}

	G_OBJECT_CLASS (eom_fullscreen_plugin_parent_class)->dispose (object);
}

static void
eom_fullscreen_plugin_activate (EomWindowActivatable *activatable)
{
	EomFullscreenPlugin *plugin = EOM_FULLSCREEN_PLUGIN (activatable);
	GtkWidget *view = eom_window_get_view (plugin->window);

	eom_debug (DEBUG_PLUGINS);

	plugin->signal_id = g_signal_connect (G_OBJECT (view),
	                                      "button-press-event",
	                                      G_CALLBACK (on_button_press),
	                                      plugin->window);
}

static void
eom_fullscreen_plugin_deactivate (EomWindowActivatable *activatable)
{
	EomFullscreenPlugin *plugin = EOM_FULLSCREEN_PLUGIN (activatable);
	GtkWidget *view = eom_window_get_view (plugin->window);

	g_signal_handler_disconnect (view, plugin->signal_id);
}

static void
eom_fullscreen_plugin_class_init (EomFullscreenPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = eom_fullscreen_plugin_dispose;
	object_class->set_property = eom_fullscreen_plugin_set_property;
	object_class->get_property = eom_fullscreen_plugin_get_property;

	g_object_class_override_property (object_class, PROP_WINDOW, "window");
}

static void
eom_fullscreen_plugin_class_finalize (EomFullscreenPluginClass *klass)
{
	/* dummy function - used by G_DEFINE_DYNAMIC_TYPE_EXTENDED */
}

static void
eom_window_activatable_iface_init (EomWindowActivatableInterface *iface)
{
	iface->activate = eom_fullscreen_plugin_activate;
	iface->deactivate = eom_fullscreen_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
	eom_fullscreen_plugin_register_type (G_TYPE_MODULE (module));
	peas_object_module_register_extension_type (module,
	                                            EOM_TYPE_WINDOW_ACTIVATABLE,
	                                            EOM_TYPE_FULLSCREEN_PLUGIN);
}
