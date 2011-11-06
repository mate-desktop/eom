#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-fullscreen-plugin.h"

#include <gmodule.h>
#include <glib/gi18n-lib.h>

#include <eom-debug.h>
#include <eom-scroll-view.h>

#define WINDOW_DATA_KEY "EomFullscreenWindowData"

EOM_PLUGIN_REGISTER_TYPE(EomFullscreenPlugin, eom_fullscreen_plugin)

typedef struct
{
	gulong signal_id;
} WindowData;

static gboolean
on_button_press (GtkWidget *button, GdkEventButton *event, EomWindow *window)
{
	if (event->button == 1 && event->type == GDK_2BUTTON_PRESS) {
		EomWindowMode mode = eom_window_get_mode (window);

		if (mode == EOM_WINDOW_MODE_SLIDESHOW ||
		    mode == EOM_WINDOW_MODE_FULLSCREEN)
			eom_window_set_mode (window, EOM_WINDOW_MODE_NORMAL);
		else if (mode == EOM_WINDOW_MODE_NORMAL)
			eom_window_set_mode (window, EOM_WINDOW_MODE_FULLSCREEN);

		return TRUE;
	}

	return FALSE;
}

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	eom_debug (DEBUG_PLUGINS);

	g_free (data);
}

static void
eom_fullscreen_plugin_init (EomFullscreenPlugin *plugin)
{
	eom_debug_message (DEBUG_PLUGINS, "EomFullscreenPlugin initializing");
}

static void
eom_fullscreen_plugin_finalize (GObject *object)
{
	eom_debug_message (DEBUG_PLUGINS, "EomFullscreenPlugin finalizing");

	G_OBJECT_CLASS (eom_fullscreen_plugin_parent_class)->finalize (object);
}

static void
impl_activate (EomPlugin *plugin,
	       EomWindow *window)
{
	GtkWidget *view = eom_window_get_view (window);
	WindowData *data;

	eom_debug (DEBUG_PLUGINS);

	data = g_new (WindowData, 1);

	data->signal_id = g_signal_connect (G_OBJECT (view),
			   		    "button-press-event",
			  		    G_CALLBACK (on_button_press),
			  		    window);

	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY,
				data,
				(GDestroyNotify) free_window_data);
}

static void
impl_deactivate	(EomPlugin *plugin,
		 EomWindow *window)
{
	GtkWidget *view = eom_window_get_view (window);
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);

	g_signal_handler_disconnect (view, data->signal_id);

	g_object_set_data (G_OBJECT (window),
			   WINDOW_DATA_KEY,
			   NULL);
}

static void
impl_update_ui (EomPlugin *plugin,
		EomWindow *window)
{
}

static void
eom_fullscreen_plugin_class_init (EomFullscreenPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	EomPluginClass *plugin_class = EOM_PLUGIN_CLASS (klass);

	object_class->finalize = eom_fullscreen_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;
}
