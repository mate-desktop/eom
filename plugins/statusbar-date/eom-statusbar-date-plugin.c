/* Statusbar Date -- Shows the EXIF date in EOM's statusbar
 *
 * Copyright (C) 2008 The Free Software Foundation
 *
 * Author: Claudio Saavedra  <csaavedra@gnome.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-statusbar-date-plugin.h"

#include <gmodule.h>
#include <glib/gi18n-lib.h>
#include <libpeas/peas-activatable.h>

#include <eom-debug.h>
#include <eom-image.h>
#include <eom-thumb-view.h>
#include <eom-exif-util.h>
#include <eom-window.h>
#include <eom-window-activatable.h>

static void eom_window_activatable_iface_init (EomWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (EomStatusbarDatePlugin,
                                eom_statusbar_date_plugin,
                                PEAS_TYPE_EXTENSION_BASE,
                                0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (EOM_TYPE_WINDOW_ACTIVATABLE,
                                                               eom_window_activatable_iface_init))

enum {
	PROP_0,
	PROP_WINDOW
};

static void
statusbar_set_date (GtkStatusbar *statusbar,
                    EomThumbView *view)
{
	EomImage *image;
	gchar *date = NULL;
	gchar time_buffer[32];
	ExifData *exif_data;

	if (eom_thumb_view_get_n_selected (view) == 0)
	{
		return;
	}

	image = eom_thumb_view_get_first_selected_image (view);

	gtk_statusbar_pop (statusbar, 0);

	if (!eom_image_has_data (image, EOM_IMAGE_DATA_EXIF))
	{
		if (!eom_image_load (image, EOM_IMAGE_DATA_EXIF, NULL, NULL))
		{
			gtk_widget_hide (GTK_WIDGET (statusbar));
		}
	}

	exif_data = eom_image_get_exif_info (image);

	if (exif_data)
	{
		date = eom_exif_util_format_date (eom_exif_data_get_value (exif_data, EXIF_TAG_DATE_TIME_ORIGINAL, time_buffer, 32));
		eom_exif_data_free (exif_data);
	}

	if (date)
	{
		gtk_statusbar_push (statusbar, 0, date);
		gtk_widget_show (GTK_WIDGET (statusbar));
		g_free (date);
	}
	else
	{
		gtk_widget_hide (GTK_WIDGET (statusbar));
	}
}

static void
selection_changed_cb (EomThumbView           *view,
                      EomStatusbarDatePlugin *plugin)
{
	statusbar_set_date (GTK_STATUSBAR (plugin->statusbar_date), view);
}

static void
eom_statusbar_date_plugin_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
	EomStatusbarDatePlugin *plugin = EOM_STATUSBAR_DATE_PLUGIN (object);

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
eom_statusbar_date_plugin_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
	EomStatusbarDatePlugin *plugin = EOM_STATUSBAR_DATE_PLUGIN (object);

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
eom_statusbar_date_plugin_init (EomStatusbarDatePlugin *plugin)
{
	eom_debug_message (DEBUG_PLUGINS, "EomStatusbarDatePlugin initializing");
}

static void
eom_statusbar_date_plugin_dispose (GObject *object)
{
	EomStatusbarDatePlugin *plugin = EOM_STATUSBAR_DATE_PLUGIN (object);

	eom_debug_message (DEBUG_PLUGINS, "EomStatusbarDatePlugin disposing");

	if (plugin->window != NULL) {
		g_object_unref (plugin->window);
		plugin->window = NULL;
	}

	G_OBJECT_CLASS (eom_statusbar_date_plugin_parent_class)->dispose (object);
}

static void
eom_statusbar_date_plugin_activate (EomWindowActivatable *activatable)
{
	EomStatusbarDatePlugin *plugin = EOM_STATUSBAR_DATE_PLUGIN (activatable);
	EomWindow *window = plugin->window;
	GtkWidget *statusbar = eom_window_get_statusbar (window);
	GtkWidget *thumbview = eom_window_get_thumb_view (window);

	eom_debug (DEBUG_PLUGINS);

	plugin->statusbar_date = gtk_statusbar_new ();
	gtk_widget_set_size_request (plugin->statusbar_date, 200, 10);
	gtk_widget_set_margin_top (GTK_WIDGET (plugin->statusbar_date), 0);
	gtk_widget_set_margin_bottom (GTK_WIDGET (plugin->statusbar_date), 0);
	gtk_box_pack_end (GTK_BOX (statusbar), plugin->statusbar_date, FALSE, FALSE, 0);

	plugin->signal_id = g_signal_connect_after (G_OBJECT (thumbview), "selection_changed",
	                                            G_CALLBACK (selection_changed_cb), plugin);

	statusbar_set_date (GTK_STATUSBAR (plugin->statusbar_date),
	                    EOM_THUMB_VIEW (eom_window_get_thumb_view (window)));
}

static void
eom_statusbar_date_plugin_deactivate (EomWindowActivatable *activatable)
{
	EomStatusbarDatePlugin *plugin = EOM_STATUSBAR_DATE_PLUGIN (activatable);
	EomWindow *window = plugin->window;
	GtkWidget *statusbar = eom_window_get_statusbar (window);
	GtkWidget *view = eom_window_get_thumb_view (window);

	g_signal_handler_disconnect (view, plugin->signal_id);

	gtk_container_remove (GTK_CONTAINER (statusbar), plugin->statusbar_date);
}

static void
eom_statusbar_date_plugin_class_init (EomStatusbarDatePluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = eom_statusbar_date_plugin_dispose;
	object_class->set_property = eom_statusbar_date_plugin_set_property;
	object_class->get_property = eom_statusbar_date_plugin_get_property;

	g_object_class_override_property (object_class, PROP_WINDOW, "window");
}

static void
eom_statusbar_date_plugin_class_finalize (EomStatusbarDatePluginClass *klass)
{
	/* dummy function - used by G_DEFINE_DYNAMIC_TYPE_EXTENDED */
}

static void
eom_window_activatable_iface_init (EomWindowActivatableInterface *iface)
{
	iface->activate = eom_statusbar_date_plugin_activate;
	iface->deactivate = eom_statusbar_date_plugin_deactivate;
}

G_MODULE_EXPORT void
peas_register_types (PeasObjectModule *module)
{
	eom_statusbar_date_plugin_register_type (G_TYPE_MODULE (module));
	peas_object_module_register_extension_type (module,
	                                            EOM_TYPE_WINDOW_ACTIVATABLE,
	                                            EOM_TYPE_STATUSBAR_DATE_PLUGIN);
}
