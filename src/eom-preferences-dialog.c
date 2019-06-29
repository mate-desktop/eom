/* Eye Of Mate - EOM Preferences Dialog
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on code by:
 *	- Jens Finke <jens@gnome.org>
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
#include "config.h"
#endif

#include "eom-preferences-dialog.h"
#include "eom-scroll-view.h"
#include "eom-util.h"
#include "eom-config-keys.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libpeas-gtk/peas-gtk-plugin-manager.h>

#define GSETTINGS_OBJECT_KEY		"GSETTINGS_KEY"
#define GSETTINGS_OBJECT_VALUE		"GSETTINGS_VALUE"

struct _EomPreferencesDialogPrivate {
	GSettings    *view_settings;
	GSettings    *ui_settings;
	GSettings    *fullscreen_settings;

	GtkWidget     *interpolate_check;
	GtkWidget     *extrapolate_check;
	GtkWidget     *autorotate_check;
	GtkWidget     *bg_color_check;
	GtkWidget     *bg_color_button;
	GtkWidget     *color_radio;
	GtkWidget     *checkpattern_radio;
	GtkWidget     *background_radio;
	GtkWidget     *transp_color_button;

	GtkWidget     *upscale_check;
	GtkWidget     *random_check;
	GtkWidget     *loop_check;
	GtkWidget     *seconds_spin;

	GtkWidget     *plugin_manager;
};

static GObject *instance = NULL;

G_DEFINE_TYPE_WITH_PRIVATE (EomPreferencesDialog, eom_preferences_dialog, GTK_TYPE_DIALOG);

static gboolean
pd_string_to_rgba_mapping (GValue   *value,
			    GVariant *variant,
			    gpointer user_data)
{
	GdkRGBA color;

	g_return_val_if_fail (g_variant_is_of_type (variant, G_VARIANT_TYPE_STRING), FALSE);

	if (gdk_rgba_parse (&color, g_variant_get_string (variant, NULL))) {
		g_value_set_boxed (value, &color);
		return TRUE;
	}

	return FALSE;
}

static GVariant*
pd_rgba_to_string_mapping (const GValue       *value,
			    const GVariantType *expected_type,
			    gpointer            user_data)
{

	GVariant *variant = NULL;
	GdkRGBA *color;
	gchar *hex_val;

	g_return_val_if_fail (G_VALUE_TYPE (value) == GDK_TYPE_RGBA, NULL);
	g_return_val_if_fail (g_variant_type_equal (expected_type, G_VARIANT_TYPE_STRING), NULL);

	color = g_value_get_boxed (value);
	hex_val = gdk_rgba_to_string(color);

	variant = g_variant_new_string (hex_val);
	g_free (hex_val);

	return variant;
}

static void
pd_transp_radio_toggle_cb (GtkWidget *widget, gpointer data)
{
	gpointer value = NULL;

	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	    return;

	value = g_object_get_data (G_OBJECT (widget), GSETTINGS_OBJECT_VALUE);

	g_settings_set_enum (G_SETTINGS (data), EOM_CONF_VIEW_TRANSPARENCY,
			     GPOINTER_TO_INT (value));
}

static void
random_change_cb (GSettings *settings, gchar *key, GtkWidget *widget)
{
	gtk_widget_set_sensitive (widget, !g_settings_get_boolean (settings, key));
}

static void
eom_preferences_response_cb (GtkDialog *dlg, gint res_id, gpointer data)
{
	switch (res_id) {
		case GTK_RESPONSE_HELP:
			eom_util_show_help ("eom-prefs", NULL);
			break;
		default:
			gtk_widget_destroy (GTK_WIDGET (dlg));
			instance = NULL;
	}
}

static void
eom_preferences_dialog_class_init (EomPreferencesDialogClass *klass)
{
	GtkWidgetClass *widget_class = (GtkWidgetClass*) klass;

	/* This should make sure the libpeas-gtk dependency isn't
	 * dropped by aggressive linkers (#739618) */
	g_type_ensure (PEAS_GTK_TYPE_PLUGIN_MANAGER);

	gtk_widget_class_set_template_from_resource (widget_class,
	                                             "/org/mate/eom/ui/eom-preferences-dialog.ui");
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              interpolate_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              extrapolate_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              autorotate_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              bg_color_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              bg_color_button);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              color_radio);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              checkpattern_radio);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              background_radio);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              transp_color_button);

	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              upscale_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              random_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              loop_check);
	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              seconds_spin);

	gtk_widget_class_bind_template_child_private (widget_class,
	                                              EomPreferencesDialog,
	                                              plugin_manager);
}

static void
eom_preferences_dialog_init (EomPreferencesDialog *pref_dlg)
{
	EomPreferencesDialogPrivate *priv;

	pref_dlg->priv = eom_preferences_dialog_get_instance_private (pref_dlg);
	priv = pref_dlg->priv;

	gtk_widget_init_template (GTK_WIDGET (pref_dlg));

	priv->view_settings = g_settings_new (EOM_CONF_VIEW);
	priv->fullscreen_settings = g_settings_new (EOM_CONF_FULLSCREEN);

	g_signal_connect (G_OBJECT (pref_dlg),
	                  "response",
	                  G_CALLBACK (eom_preferences_response_cb),
	                  pref_dlg);

	g_settings_bind (priv->view_settings,
	                 EOM_CONF_VIEW_INTERPOLATE,
	                 priv->interpolate_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
	                 EOM_CONF_VIEW_EXTRAPOLATE,
	                 priv->extrapolate_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
	                 EOM_CONF_VIEW_AUTOROTATE,
	                 priv->autorotate_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
	                 EOM_CONF_VIEW_USE_BG_COLOR,
	                 priv->bg_color_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind_with_mapping (priv->view_settings,
	                              EOM_CONF_VIEW_BACKGROUND_COLOR,
	                              priv->bg_color_button, "rgba",
	                              G_SETTINGS_BIND_DEFAULT,
	                              pd_string_to_rgba_mapping,
	                              pd_rgba_to_string_mapping,
	                              NULL, NULL);
	g_object_set_data (G_OBJECT (priv->color_radio),
	                   GSETTINGS_OBJECT_VALUE,
	                   GINT_TO_POINTER (EOM_TRANSP_COLOR));

	g_signal_connect (G_OBJECT (priv->color_radio),
	                  "toggled",
	                  G_CALLBACK (pd_transp_radio_toggle_cb),
	                  priv->view_settings);

	g_object_set_data (G_OBJECT (priv->checkpattern_radio),
	                   GSETTINGS_OBJECT_VALUE,
	                   GINT_TO_POINTER (EOM_TRANSP_CHECKED));

	g_signal_connect (G_OBJECT (priv->checkpattern_radio),
	                  "toggled",
	                  G_CALLBACK (pd_transp_radio_toggle_cb),
	                  priv->view_settings);

	g_object_set_data (G_OBJECT (priv->background_radio),
	                   GSETTINGS_OBJECT_VALUE,
	                   GINT_TO_POINTER (EOM_TRANSP_BACKGROUND));

	g_signal_connect (G_OBJECT (priv->background_radio),
	                  "toggled",
	                  G_CALLBACK (pd_transp_radio_toggle_cb),
	                  priv->view_settings);

	switch (g_settings_get_enum (priv->view_settings,
				    EOM_CONF_VIEW_TRANSPARENCY))
	{
	case EOM_TRANSP_COLOR:
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->color_radio), TRUE);
		break;
	case EOM_TRANSP_CHECKED:
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->checkpattern_radio), TRUE);
		break;
	default:
		// Log a warning and use EOM_TRANSP_BACKGROUND as fallback
		g_warn_if_reached ();
	case EOM_TRANSP_BACKGROUND:
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->background_radio), TRUE);
		break;
	}

	g_settings_bind_with_mapping (priv->view_settings,
	                              EOM_CONF_VIEW_TRANS_COLOR,
	                              priv->transp_color_button, "rgba",
	                              G_SETTINGS_BIND_DEFAULT,
	                              pd_string_to_rgba_mapping,
	                              pd_rgba_to_string_mapping,
	                              NULL, NULL);

	g_settings_bind (priv->fullscreen_settings, EOM_CONF_FULLSCREEN_UPSCALE,
	                 priv->upscale_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (priv->fullscreen_settings,
	                 EOM_CONF_FULLSCREEN_LOOP,
	                 priv->loop_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (priv->fullscreen_settings,
	                 EOM_CONF_FULLSCREEN_RANDOM,
	                 priv->random_check, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_signal_connect (priv->fullscreen_settings,
	                  "changed::" EOM_CONF_FULLSCREEN_RANDOM,
	                  G_CALLBACK (random_change_cb),
	                  priv->loop_check);
	random_change_cb (priv->fullscreen_settings,
	                  EOM_CONF_FULLSCREEN_RANDOM,
	                  priv->loop_check);

	g_settings_bind (priv->fullscreen_settings,
	                 EOM_CONF_FULLSCREEN_SECONDS,
	                 priv->seconds_spin, "value",
	                 G_SETTINGS_BIND_DEFAULT);

	gtk_widget_show_all (priv->plugin_manager);
}

GtkWidget *eom_preferences_dialog_get_instance (GtkWindow *parent)
{
	if (instance == NULL) {
		instance = g_object_new (EOM_TYPE_PREFERENCES_DIALOG,
				 	 NULL);
	}

	if (parent)
		gtk_window_set_transient_for (GTK_WINDOW (instance), parent);

	return GTK_WIDGET(instance);
}
