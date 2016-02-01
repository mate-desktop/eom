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
#include "eom-plugin-manager.h"
#include "eom-scroll-view.h"
#include "eom-util.h"
#include "eom-config-keys.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include <libmate-desktop/mate-colorbutton.h>

#define EOM_PREFERENCES_DIALOG_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EOM_TYPE_PREFERENCES_DIALOG, EomPreferencesDialogPrivate))

G_DEFINE_TYPE (EomPreferencesDialog, eom_preferences_dialog, EOM_TYPE_DIALOG);

#define GSETTINGS_OBJECT_KEY		"GSETTINGS_KEY"
#define GSETTINGS_OBJECT_VALUE		"GSETTINGS_VALUE"

struct _EomPreferencesDialogPrivate {
	GSettings    *view_settings;
	GSettings    *ui_settings;
	GSettings    *fullscreen_settings;
};

static GObject *instance = NULL;

static gboolean
pd_string_to_color_mapping (GValue   *value,
			    GVariant *variant,
			    gpointer user_data)
{
	GdkColor color;

	g_return_val_if_fail (g_variant_is_of_type (variant, G_VARIANT_TYPE_STRING), FALSE);

	if (gdk_color_parse (g_variant_get_string (variant, NULL), &color)) {
		g_value_set_boxed (value, &color);
		return TRUE;
	}

	return FALSE;
}

static GVariant*
pd_color_to_string_mapping (const GValue       *value,
			    const GVariantType *expected_type,
			    gpointer            user_data)
{

	GVariant *variant = NULL;
	GdkColor *color;
	gchar *hex_val;

	g_return_val_if_fail (G_VALUE_TYPE (value) == GDK_TYPE_COLOR, NULL);
	g_return_val_if_fail (g_variant_type_equal (expected_type, G_VARIANT_TYPE_STRING), NULL);

	color = g_value_get_boxed (value);
	hex_val = g_strdup_printf ("#%02X%02X%02X",
				 color->red / 256,
				 color->green / 256,
				 color->blue / 256);
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

static GObject *
eom_preferences_dialog_constructor (GType type,
				    guint n_construct_properties,
				    GObjectConstructParam *construct_params)

{
	EomPreferencesDialogPrivate *priv;
	GtkWidget *dlg;
	GtkWidget *interpolate_check;
	GtkWidget *extrapolate_check;
	GtkWidget *autorotate_check;
	GtkWidget *bg_color_check;
	GtkWidget *bg_color_button;
	GtkWidget *color_radio;
	GtkWidget *checkpattern_radio;
	GtkWidget *background_radio;
	GtkWidget *color_button;
	GtkWidget *upscale_check;
	GtkWidget *random_check;
	GtkWidget *loop_check;
	GtkWidget *seconds_spin;
	GtkWidget *plugin_manager;
	GtkWidget *plugin_manager_container;
	GObject *object;

	object = G_OBJECT_CLASS (eom_preferences_dialog_parent_class)->constructor
			(type, n_construct_properties, construct_params);

	priv = EOM_PREFERENCES_DIALOG (object)->priv;

	priv->view_settings = g_settings_new (EOM_CONF_VIEW);
	priv->fullscreen_settings = g_settings_new (EOM_CONF_FULLSCREEN);

	eom_dialog_construct (EOM_DIALOG (object),
			      "eom-preferences-dialog.ui",
			      "eom_preferences_dialog");

	eom_dialog_get_controls (EOM_DIALOG (object),
			         "eom_preferences_dialog", &dlg,
			         "interpolate_check", &interpolate_check,
			         "extrapolate_check", &extrapolate_check,
			         "autorotate_check", &autorotate_check,
			         "bg_color_check", &bg_color_check,
			         "bg_color_button", &bg_color_button,
			         "color_radio", &color_radio,
			         "checkpattern_radio", &checkpattern_radio,
			         "background_radio", &background_radio,
			         "color_button", &color_button,
			         "upscale_check", &upscale_check,
			         "random_check", &random_check,
			         "loop_check", &loop_check,
			         "seconds_spin", &seconds_spin,
			         "plugin_manager_container", &plugin_manager_container,
			         NULL);

	g_signal_connect (G_OBJECT (dlg),
			  "response",
			  G_CALLBACK (eom_preferences_response_cb),
			  dlg);

	g_settings_bind (priv->view_settings,
					 EOM_CONF_VIEW_INTERPOLATE,
					 G_OBJECT (interpolate_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
					 EOM_CONF_VIEW_EXTRAPOLATE,
					 G_OBJECT (extrapolate_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
					 EOM_CONF_VIEW_AUTOROTATE,
					 G_OBJECT (autorotate_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (priv->view_settings,
					 EOM_CONF_VIEW_USE_BG_COLOR,
					 G_OBJECT (bg_color_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind_with_mapping (priv->view_settings,
				      EOM_CONF_VIEW_BACKGROUND_COLOR,
				      bg_color_button, "color",
				      G_SETTINGS_BIND_DEFAULT,
				      pd_string_to_color_mapping,
				      pd_color_to_string_mapping,
				      NULL, NULL);

	g_object_set_data (G_OBJECT (color_radio),
			   GSETTINGS_OBJECT_VALUE,
			   GINT_TO_POINTER (EOM_TRANSP_COLOR));

	g_signal_connect (G_OBJECT (color_radio),
			  "toggled",
			  G_CALLBACK (pd_transp_radio_toggle_cb),
			  priv->view_settings);

	g_object_set_data (G_OBJECT (checkpattern_radio),
			   GSETTINGS_OBJECT_VALUE,
			   GINT_TO_POINTER (EOM_TRANSP_CHECKED));

	g_signal_connect (G_OBJECT (checkpattern_radio),
			  "toggled",
			  G_CALLBACK (pd_transp_radio_toggle_cb),
			  priv->view_settings);

	g_object_set_data (G_OBJECT (background_radio),
			   GSETTINGS_OBJECT_VALUE,
			   GINT_TO_POINTER (EOM_TRANSP_BACKGROUND));

	g_signal_connect (G_OBJECT (background_radio),
			  "toggled",
			  G_CALLBACK (pd_transp_radio_toggle_cb),
			  priv->view_settings);

	switch (g_settings_get_enum (priv->view_settings,
				    EOM_CONF_VIEW_TRANSPARENCY))
	{
	case EOM_TRANSP_COLOR:
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (color_radio), TRUE);
		break;
	case EOM_TRANSP_CHECKED:
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkpattern_radio), TRUE);
		break;
	default:
		// Log a warning and use EOM_TRANSP_BACKGROUND as fallback
		g_warn_if_reached ();
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (background_radio), TRUE);
		break;
	}

	g_settings_bind_with_mapping (priv->view_settings,
				      EOM_CONF_VIEW_TRANS_COLOR,
				      color_button, "color",
				      G_SETTINGS_BIND_DEFAULT,
				      pd_string_to_color_mapping,
				      pd_color_to_string_mapping,
				      NULL, NULL);

	g_settings_bind (priv->fullscreen_settings, EOM_CONF_FULLSCREEN_UPSCALE,
			 upscale_check, "active", G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (priv->fullscreen_settings,
					 EOM_CONF_FULLSCREEN_LOOP,
					 G_OBJECT (loop_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (priv->fullscreen_settings,
					 EOM_CONF_FULLSCREEN_RANDOM,
					 G_OBJECT (random_check),
					 "active",
					 G_SETTINGS_BIND_DEFAULT);
	g_signal_connect (priv->fullscreen_settings,
					  "changed::" EOM_CONF_FULLSCREEN_RANDOM,
					  G_CALLBACK (random_change_cb),
					  loop_check);
	random_change_cb (priv->fullscreen_settings,
					  EOM_CONF_FULLSCREEN_RANDOM,
					  loop_check);

	g_settings_bind (priv->fullscreen_settings,
					 EOM_CONF_FULLSCREEN_SECONDS,
					 G_OBJECT (seconds_spin),
					 "value",
					 G_SETTINGS_BIND_DEFAULT);

        plugin_manager = eom_plugin_manager_new ();

        g_assert (plugin_manager != NULL);

        gtk_box_pack_start (GTK_BOX (plugin_manager_container),
                            plugin_manager,
                            TRUE,
                            TRUE,
                            0);

        gtk_widget_show_all (plugin_manager);

	return object;
}

static void
eom_preferences_dialog_dispose (GObject *object)
{
	EomPreferencesDialog *pref_dlg = EOM_PREFERENCES_DIALOG (object);

	g_object_unref (pref_dlg->priv->view_settings);
	g_object_unref (pref_dlg->priv->fullscreen_settings);
	g_object_unref (pref_dlg->priv->ui_settings);

	G_OBJECT_CLASS (eom_preferences_dialog_parent_class)->dispose (object);
}

static void
eom_preferences_dialog_class_init (EomPreferencesDialogClass *class)
{
	GObjectClass *g_object_class = (GObjectClass *) class;

	g_object_class->constructor = eom_preferences_dialog_constructor;
	g_object_class->dispose = eom_preferences_dialog_dispose;

	g_type_class_add_private (g_object_class, sizeof (EomPreferencesDialogPrivate));
}

static void
eom_preferences_dialog_init (EomPreferencesDialog *pref_dlg)
{
	pref_dlg->priv = EOM_PREFERENCES_DIALOG_GET_PRIVATE (pref_dlg);

	pref_dlg->priv->view_settings = g_settings_new (EOM_CONF_VIEW);
	pref_dlg->priv->fullscreen_settings = g_settings_new (EOM_CONF_FULLSCREEN);
	pref_dlg->priv->ui_settings = g_settings_new (EOM_CONF_UI);
}

GObject *
eom_preferences_dialog_get_instance (GtkWindow *parent)
{
	if (instance == NULL) {
		instance = g_object_new (EOM_TYPE_PREFERENCES_DIALOG,
				 	 "parent-window", parent,
				 	 NULL);
	}

	return instance;
}
