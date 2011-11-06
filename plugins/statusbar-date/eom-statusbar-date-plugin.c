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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-statusbar-date-plugin.h"

#include <gmodule.h>
#include <glib/gi18n-lib.h>

#include <libexif/exif-data.h>

#include <eom-debug.h>
#include <eom-scroll-view.h>
#include <eom-image.h>
#include <eom-thumb-view.h>
#include <eom-exif-util.h>


#define WINDOW_DATA_KEY "EomStatusbarDateWindowData"

EOM_PLUGIN_REGISTER_TYPE(EomStatusbarDatePlugin, eom_statusbar_date_plugin)

typedef struct
{
	GtkWidget *statusbar_date;
	gulong signal_id;
} WindowData;

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	eom_debug (DEBUG_PLUGINS);

	g_free (data);
}

static void
statusbar_set_date (GtkStatusbar *statusbar, EomThumbView *view)
{
	EomImage *image;
	gchar *date = NULL;
	gchar time_buffer[32];
	ExifData *exif_data;

	if (eom_thumb_view_get_n_selected (view) == 0)
		return;

	image = eom_thumb_view_get_first_selected_image (view);

	gtk_statusbar_pop (statusbar, 0);

	if (!eom_image_has_data (image, EOM_IMAGE_DATA_EXIF)) {
		if (!eom_image_load (image, EOM_IMAGE_DATA_EXIF, NULL, NULL)) {
			gtk_widget_hide (GTK_WIDGET (statusbar));
		}
	}

	exif_data = (ExifData *) eom_image_get_exif_info (image);
	if (exif_data) {
		date = eom_exif_util_format_date (
			eom_exif_util_get_value (exif_data, EXIF_TAG_DATE_TIME_ORIGINAL, time_buffer, 32));
		exif_data_unref (exif_data);
	}

	if (date) {
		gtk_statusbar_push (statusbar, 0, date);
		gtk_widget_show (GTK_WIDGET (statusbar));
		g_free (date);
	} else {
		gtk_widget_hide (GTK_WIDGET (statusbar));
	}
}

static void
selection_changed_cb (EomThumbView *view, WindowData *data)
{
	statusbar_set_date (GTK_STATUSBAR (data->statusbar_date), view);
}
static void
eom_statusbar_date_plugin_init (EomStatusbarDatePlugin *plugin)
{
	eom_debug_message (DEBUG_PLUGINS, "EomStatusbarDatePlugin initializing");
}

static void
eom_statusbar_date_plugin_finalize (GObject *object)
{
	eom_debug_message (DEBUG_PLUGINS, "EomStatusbarDatePlugin finalizing");

	G_OBJECT_CLASS (eom_statusbar_date_plugin_parent_class)->finalize (object);
}

static void
impl_activate (EomPlugin *plugin,
	       EomWindow *window)
{
	GtkWidget *statusbar = eom_window_get_statusbar (window);
	GtkWidget *thumbview = eom_window_get_thumb_view (window);
	WindowData *data;

	eom_debug (DEBUG_PLUGINS);

	data = g_new (WindowData, 1);
	data->statusbar_date = gtk_statusbar_new ();
	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (data->statusbar_date),
					   FALSE);
	gtk_widget_set_size_request (data->statusbar_date, 200, 10);
	gtk_box_pack_end (GTK_BOX (statusbar),
			  data->statusbar_date,
			  FALSE, FALSE, 0);

	data->signal_id = g_signal_connect_after (G_OBJECT (thumbview), "selection_changed",
						  G_CALLBACK (selection_changed_cb), data);

	statusbar_set_date (GTK_STATUSBAR (data->statusbar_date),
			    EOM_THUMB_VIEW (eom_window_get_thumb_view (window)));

	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY,
				data,
				(GDestroyNotify) free_window_data);
}

static void
impl_deactivate	(EomPlugin *plugin,
		 EomWindow *window)
{
	GtkWidget *statusbar = eom_window_get_statusbar (window);
	GtkWidget *view = eom_window_get_thumb_view (window);
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);

	g_signal_handler_disconnect (view, data->signal_id);

	gtk_container_remove (GTK_CONTAINER (statusbar), data->statusbar_date);

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
eom_statusbar_date_plugin_class_init (EomStatusbarDatePluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	EomPluginClass *plugin_class = EOM_PLUGIN_CLASS (klass);

	object_class->finalize = eom_statusbar_date_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;
}
