/*
 * eom-metadata-sidebar.c
 * This file is part of eom
 *
 * Author: Felix Riemann <friemann@gnome.org>
 *
 * Portions based on code by: Lucas Rocha <lucasr@gnome.org>
 *                            Hubert Figuiere <hub@figuiere.net> (XMP support)
 *
 * Copyright (C) 2011 GNOME Foundation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "eom-image.h"
#include "eom-metadata-sidebar.h"
#include "eom-properties-dialog.h"
#include "eom-scroll-view.h"
#include "eom-util.h"
#include "eom-window.h"

#ifdef HAVE_EXIF
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>
#include "eom-exif-util.h"
#endif

#if HAVE_EXEMPI
#include <exempi/xmp.h>
#include <exempi/xmpconsts.h>
#endif

/* There's no exempi support in the sidebar yet */
#if HAVE_EXIF  /*|| HAVE_EXEMPI */
#define HAVE_METADATA 1
#endif

enum {
	PROP_0,
	PROP_IMAGE,
	PROP_PARENT_WINDOW
};

struct _EomMetadataSidebarPrivate {
	EomWindow *parent_window;
	EomImage *image;

	gulong image_changed_id;
	gulong thumb_changed_id;

	GtkWidget *size_label;
	GtkWidget *type_label;
	GtkWidget *filesize_label;
	GtkWidget *folder_label;

#if HAVE_EXIF
	GtkWidget *aperture_label;
	GtkWidget *exposure_label;
	GtkWidget *focallen_label;
	GtkWidget *iso_label;
	GtkWidget *metering_label;
	GtkWidget *model_label;
	GtkWidget *date_label;
	GtkWidget *time_label;
#else
	GtkWidget *metadata_grid;
#endif

#if HAVE_METADATA
	GtkWidget *details_button;
#endif
};

G_DEFINE_TYPE_WITH_PRIVATE(EomMetadataSidebar, eom_metadata_sidebar, GTK_TYPE_SCROLLED_WINDOW)

static void
parent_file_display_name_query_info_cb (GObject *source_object,
                                        GAsyncResult *res,
                                        gpointer user_data)
{
	EomMetadataSidebar *sidebar = EOM_METADATA_SIDEBAR (user_data);
	GFile *parent_file = G_FILE (source_object);
	GFileInfo *file_info;
	gchar *baseuri;
	gchar *display_name;
	gchar *str;

	file_info = g_file_query_info_finish (parent_file, res, NULL);
	if (file_info == NULL) {
		display_name = g_file_get_basename (parent_file);
	} else {
		display_name = g_strdup (
			g_file_info_get_display_name (file_info));
		g_object_unref (file_info);
	}
	baseuri = g_file_get_uri (parent_file);
	str = g_markup_printf_escaped ("<a href=\"%s\">%s</a>",
				       baseuri,
				       display_name);
	gtk_label_set_markup (GTK_LABEL (sidebar->priv->folder_label), str);

	g_free (str);
	g_free (baseuri);
	g_free (display_name);

	str = g_file_get_path (parent_file);
	gtk_widget_set_tooltip_text (GTK_WIDGET (sidebar->priv->folder_label), str);
	g_free (str);

	g_object_unref (sidebar);
}

static void
eom_metadata_sidebar_update_general_section (EomMetadataSidebar *sidebar)
{
	EomMetadataSidebarPrivate *priv = sidebar->priv;
	EomImage *img = priv->image;
	GFile *file, *parent_file;
	GFileInfo *file_info;
	gchar *str;
	goffset bytes;
	gint width, height;

	if (G_UNLIKELY (img == NULL)) {
		gtk_label_set_text (GTK_LABEL (priv->size_label), NULL);
		gtk_label_set_text (GTK_LABEL (priv->type_label), NULL);
		gtk_label_set_text (GTK_LABEL (priv->filesize_label), NULL);
		gtk_label_set_text (GTK_LABEL (priv->folder_label), NULL);
		return;
	}

	eom_image_get_size (img, &width, &height);
	str = g_strdup_printf (ngettext("%i × %i pixel",
					"%i × %i pixels", height),
			       width, height);
	gtk_label_set_text (GTK_LABEL (priv->size_label), str);
	g_free (str);

	file = eom_image_get_file (img);
	file_info = g_file_query_info (file,
				       G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
				       0, NULL, NULL);
	if (file_info == NULL) {
		str = g_strdup (_("Unknown"));
	} else {
		const gchar *mime_str;

		mime_str = g_file_info_get_content_type (file_info);
		str = g_content_type_get_description (mime_str);
		g_object_unref (file_info);
	}
	gtk_label_set_text (GTK_LABEL (priv->type_label), str);
	g_free (str);

	bytes = eom_image_get_bytes (img);
	str = g_format_size (bytes);
	gtk_label_set_text (GTK_LABEL (priv->filesize_label), str);
	g_free (str);

	parent_file = g_file_get_parent (file);
	if (parent_file == NULL) {
		/* file is root directory itself */
		parent_file = g_object_ref (file);
	}
	gtk_label_set_markup (GTK_LABEL (sidebar->priv->folder_label), NULL);
	g_file_query_info_async (parent_file,
				 G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
				 G_FILE_QUERY_INFO_NONE,
				 G_PRIORITY_DEFAULT,
				 NULL,
				 parent_file_display_name_query_info_cb,
				 g_object_ref (sidebar));

	g_object_unref (parent_file);
}

#if HAVE_METADATA
static void
eom_metadata_sidebar_update_metadata_section (EomMetadataSidebar *sidebar)
{
	EomMetadataSidebarPrivate *priv = sidebar->priv;
#if HAVE_EXIF
	EomImage *img = priv->image;
	ExifData *exif_data = NULL;

	if (img) {
		exif_data = eom_image_get_exif_info (img);
	}

	eom_exif_util_set_label_text (GTK_LABEL (priv->aperture_label),
				      exif_data, EXIF_TAG_FNUMBER);
	eom_exif_util_set_label_text (GTK_LABEL (priv->exposure_label),
				      exif_data,
				      EXIF_TAG_EXPOSURE_TIME);
	eom_exif_util_set_focal_length_label_text (
				       GTK_LABEL (priv->focallen_label),
				       exif_data);
	eom_exif_util_set_label_text (GTK_LABEL (priv->iso_label),
				      exif_data,
				      EXIF_TAG_ISO_SPEED_RATINGS);
	eom_exif_util_set_label_text (GTK_LABEL (priv->metering_label),
				      exif_data,
				      EXIF_TAG_METERING_MODE);
	eom_exif_util_set_label_text (GTK_LABEL (priv->model_label),
				      exif_data, EXIF_TAG_MODEL);
	eom_exif_util_format_datetime_label (GTK_LABEL (priv->date_label),
				      exif_data,
				      EXIF_TAG_DATE_TIME_ORIGINAL,
				      _("%a, %d %B %Y"));
	eom_exif_util_format_datetime_label (GTK_LABEL (priv->time_label),
				      exif_data,
				      EXIF_TAG_DATE_TIME_ORIGINAL,
				      _("%X"));

	/* exif_data_unref can handle NULL-values */
	exif_data_unref(exif_data);
#endif /* HAVE_EXIF */
}
#endif /* HAVE_METADATA */

static void
eom_metadata_sidebar_update (EomMetadataSidebar *sidebar)
{
	g_return_if_fail (EOM_IS_METADATA_SIDEBAR (sidebar));

	eom_metadata_sidebar_update_general_section (sidebar);
#if HAVE_METADATA
	eom_metadata_sidebar_update_metadata_section (sidebar);
#endif
}

static void
_thumbnail_changed_cb (EomImage *image, gpointer user_data)
{
	eom_metadata_sidebar_update (EOM_METADATA_SIDEBAR (user_data));
}

static void
eom_metadata_sidebar_set_image (EomMetadataSidebar *sidebar, EomImage *image)
{
	EomMetadataSidebarPrivate *priv = sidebar->priv;

	if (image == priv->image)
		return;


	if (priv->thumb_changed_id != 0) {
		g_signal_handler_disconnect (priv->image,
					     priv->thumb_changed_id);
		priv->thumb_changed_id = 0;
	}

	if (priv->image)
		g_object_unref (priv->image);

	priv->image = image;

	if (priv->image) {
		g_object_ref (priv->image);
		priv->thumb_changed_id =
			g_signal_connect (priv->image, "thumbnail-changed",
					  G_CALLBACK (_thumbnail_changed_cb),
					  sidebar);
		eom_metadata_sidebar_update (sidebar);
	}

	g_object_notify (G_OBJECT (sidebar), "image");
}

static void
_notify_image_cb (GObject *gobject, GParamSpec *pspec, gpointer user_data)
{
	EomImage *image;

	g_return_if_fail (EOM_IS_METADATA_SIDEBAR (user_data));
	g_return_if_fail (EOM_IS_SCROLL_VIEW (gobject));

	image = eom_scroll_view_get_image (EOM_SCROLL_VIEW (gobject));

	eom_metadata_sidebar_set_image (EOM_METADATA_SIDEBAR (user_data),
					image);

	if (image)
		g_object_unref (image);
}

static void
_folder_label_clicked_cb (GtkLabel *label, const gchar *uri, gpointer user_data)
{
	EomMetadataSidebarPrivate *priv = EOM_METADATA_SIDEBAR(user_data)->priv;
	EomImage *img;
	GtkWidget *toplevel;
	GtkWindow *window;
	GFile *file;

	g_return_if_fail (priv->parent_window != NULL);

	img = eom_window_get_image (priv->parent_window);
	file = eom_image_get_file (img);

	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (label));
	if (GTK_IS_WINDOW (toplevel))
		window = GTK_WINDOW (toplevel);
	else
		window = NULL;

	eom_util_show_file_in_filemanager (file, window);

	g_object_unref (file);
}

#ifdef HAVE_METADATA
static void
_details_button_clicked_cb (GtkButton *button, gpointer user_data)
{
	EomMetadataSidebarPrivate *priv = EOM_METADATA_SIDEBAR(user_data)->priv;
	GtkWidget *dlg;

	g_return_if_fail (priv->parent_window != NULL);

	dlg = eom_window_get_properties_dialog (
					EOM_WINDOW (priv->parent_window));
	g_return_if_fail (dlg != NULL);
	eom_properties_dialog_set_page (EOM_PROPERTIES_DIALOG (dlg),
					EOM_PROPERTIES_DIALOG_PAGE_DETAILS);
	gtk_widget_show (dlg);
}
#endif /* HAVE_METADATA */

static void
eom_metadata_sidebar_set_parent_window (EomMetadataSidebar *sidebar,
					EomWindow *window)
{
	EomMetadataSidebarPrivate *priv;
	GtkWidget *view;

	g_return_if_fail (EOM_IS_METADATA_SIDEBAR (sidebar));
	priv = sidebar->priv;
	g_return_if_fail (priv->parent_window == NULL);

	priv->parent_window = g_object_ref (window);
	eom_metadata_sidebar_update (sidebar);
	view = eom_window_get_view (window);
	priv->image_changed_id = g_signal_connect (view, "notify::image",
						  G_CALLBACK (_notify_image_cb),
						  sidebar);

	g_object_notify (G_OBJECT (sidebar), "parent-window");

}

static void
eom_metadata_sidebar_init (EomMetadataSidebar *sidebar)
{
	EomMetadataSidebarPrivate *priv;

	priv = sidebar->priv = eom_metadata_sidebar_get_instance_private (sidebar);

	gtk_widget_init_template (GTK_WIDGET (sidebar));

	g_signal_connect (priv->folder_label, "activate-link",
	                  G_CALLBACK (_folder_label_clicked_cb), sidebar);

#if HAVE_METADATA
	g_signal_connect (priv->details_button, "clicked",
			  G_CALLBACK (_details_button_clicked_cb), sidebar);
#endif /* HAVE_METADATA */

#ifndef HAVE_EXIF
	{
		/* Remove the lower 8 lines as they are empty without libexif*/
		guint i;

		for (i = 11; i > 3; i--)
		{
			gtk_grid_remove_row (GTK_GRID (priv->metadata_grid), i);
		}
	}
#endif /* !HAVE_EXIF */
}

static void
eom_metadata_sidebar_get_property (GObject *object, guint property_id,
				   GValue *value, GParamSpec *pspec)
{
	EomMetadataSidebar *sidebar;

	g_return_if_fail (EOM_IS_METADATA_SIDEBAR (object));

	sidebar = EOM_METADATA_SIDEBAR (object);

	switch (property_id) {
	case PROP_IMAGE:
	{
		g_value_set_object (value, sidebar->priv->image);
		break;
	}
	case PROP_PARENT_WINDOW:
		g_value_set_object (value, sidebar->priv->parent_window);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
eom_metadata_sidebar_set_property (GObject *object, guint property_id,
				   const GValue *value, GParamSpec *pspec)
{
	EomMetadataSidebar *sidebar;

	g_return_if_fail (EOM_IS_METADATA_SIDEBAR (object));

	sidebar = EOM_METADATA_SIDEBAR (object);

	switch (property_id) {
	case PROP_IMAGE:
	{
		break;
	}
	case PROP_PARENT_WINDOW:
	{
		EomWindow *window;

		window = g_value_get_object (value);
		eom_metadata_sidebar_set_parent_window (sidebar, window);
		break;
	}
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}

}
static void
eom_metadata_sidebar_class_init (EomMetadataSidebarClass *klass)
{
	GObjectClass *g_obj_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	g_obj_class->get_property = eom_metadata_sidebar_get_property;
	g_obj_class->set_property = eom_metadata_sidebar_set_property;
/*	g_obj_class->dispose = eom_metadata_sidebar_dispose;*/

	g_object_class_install_property (
		g_obj_class, PROP_PARENT_WINDOW,
		g_param_spec_object ("parent-window", NULL, NULL,
				     EOM_TYPE_WINDOW, G_PARAM_READWRITE
				     | G_PARAM_CONSTRUCT_ONLY
				     | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (
		g_obj_class, PROP_IMAGE,
		g_param_spec_object ("image", NULL, NULL, EOM_TYPE_IMAGE,
				     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)
				    );

	gtk_widget_class_set_template_from_resource (widget_class,
						     "/org/mate/eom/ui/metadata-sidebar.ui");

	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      size_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      type_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      filesize_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      folder_label);
#if HAVE_EXIF
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      aperture_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      exposure_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      focallen_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      iso_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      metering_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      model_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      date_label);
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      time_label);
#else
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      metadata_grid);
#endif /* HAVE_EXIF */
#if HAVE_METADATA
	gtk_widget_class_bind_template_child_private (widget_class,
						      EomMetadataSidebar,
						      details_button);
#endif /* HAVE_METADATA */
}


GtkWidget*
eom_metadata_sidebar_new (EomWindow *window)
{
	return gtk_widget_new (EOM_TYPE_METADATA_SIDEBAR,
			       "hadjustment", NULL,
			       "vadjustment", NULL,
			       "hscrollbar-policy", GTK_POLICY_NEVER,
			       "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
			       "parent-window", window,
			       NULL);
}
