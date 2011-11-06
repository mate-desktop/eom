/* Eye Of Mate - Thumbnail Navigator
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
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
#include "config.h"
#endif

#include "eom-thumb-nav.h"
#include "eom-thumb-view.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <string.h>

#define EOM_THUMB_NAV_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EOM_TYPE_THUMB_NAV, EomThumbNavPrivate))

G_DEFINE_TYPE (EomThumbNav, eom_thumb_nav, GTK_TYPE_HBOX);

#define EOM_THUMB_NAV_SCROLL_INC      20
#define EOM_THUMB_NAV_SCROLL_MOVE     20
#define EOM_THUMB_NAV_SCROLL_TIMEOUT  20

enum
{
	PROP_SHOW_BUTTONS = 1,
	PROP_THUMB_VIEW,
	PROP_MODE
};

struct _EomThumbNavPrivate {
	EomThumbNavMode   mode;

	gboolean          show_buttons;
	gboolean          scroll_dir;
	gint              scroll_pos;
	gint              scroll_id;

	GtkWidget        *button_left;
	GtkWidget        *button_right;
	GtkWidget        *sw;
	GtkWidget        *thumbview;
	GtkAdjustment    *adj;
};

static gboolean
eom_thumb_nav_scroll_event (GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
	EomThumbNav *nav = EOM_THUMB_NAV (user_data);
	gint inc = EOM_THUMB_NAV_SCROLL_INC * 3;

	if (nav->priv->mode != EOM_THUMB_NAV_MODE_ONE_ROW)
		return FALSE;

	switch (event->direction) {
	case GDK_SCROLL_UP:
	case GDK_SCROLL_LEFT:
		inc *= -1;
		break;

	case GDK_SCROLL_DOWN:
	case GDK_SCROLL_RIGHT:
		break;

	default:
		g_assert_not_reached ();
		return FALSE;
	}

	if (inc < 0)
		gtk_adjustment_set_value (nav->priv->adj, MAX (0, gtk_adjustment_get_value (nav->priv->adj) + inc));
	else
		gtk_adjustment_set_value (nav->priv->adj, MIN (gtk_adjustment_get_upper (nav->priv->adj) - gtk_adjustment_get_page_size (nav->priv->adj), gtk_adjustment_get_value (nav->priv->adj) + inc));

	return TRUE;
}

static void
eom_thumb_nav_adj_changed (GtkAdjustment *adj, gpointer user_data)
{
	EomThumbNav *nav;
	EomThumbNavPrivate *priv;
	gboolean ltr;

	nav = EOM_THUMB_NAV (user_data);
	priv = EOM_THUMB_NAV_GET_PRIVATE (nav);
	ltr = gtk_widget_get_direction (priv->sw) == GTK_TEXT_DIR_LTR;

	gtk_widget_set_sensitive (ltr ? priv->button_right : priv->button_left,
				  gtk_adjustment_get_value (adj)
				   < gtk_adjustment_get_upper (adj)
				    - gtk_adjustment_get_page_size (adj));
}

static void
eom_thumb_nav_adj_value_changed (GtkAdjustment *adj, gpointer user_data)
{
	EomThumbNav *nav;
	EomThumbNavPrivate *priv;
	gboolean ltr;

	nav = EOM_THUMB_NAV (user_data);
	priv = EOM_THUMB_NAV_GET_PRIVATE (nav);
	ltr = gtk_widget_get_direction (priv->sw) == GTK_TEXT_DIR_LTR;

	gtk_widget_set_sensitive (ltr ? priv->button_left : priv->button_right,
				  gtk_adjustment_get_value (adj) > 0);

	gtk_widget_set_sensitive (ltr ? priv->button_right : priv->button_left,
				  gtk_adjustment_get_value (adj)
				   < gtk_adjustment_get_upper (adj)
				    - gtk_adjustment_get_page_size (adj));
}

static gboolean
eom_thumb_nav_scroll_step (gpointer user_data)
{
	EomThumbNav *nav = EOM_THUMB_NAV (user_data);
	GtkAdjustment *adj = nav->priv->adj;
	gint delta;

	if (nav->priv->scroll_pos < 10)
		delta = EOM_THUMB_NAV_SCROLL_INC;
	else if (nav->priv->scroll_pos < 20)
		delta = EOM_THUMB_NAV_SCROLL_INC * 2;
	else if (nav->priv->scroll_pos < 30)
		delta = EOM_THUMB_NAV_SCROLL_INC * 2 + 5;
	else
		delta = EOM_THUMB_NAV_SCROLL_INC * 2 + 12;

	if (!nav->priv->scroll_dir)
		delta *= -1;

	if ((gint) (gtk_adjustment_get_value (adj) + (gdouble) delta) >= 0 &&
	    (gint) (gtk_adjustment_get_value (adj) + (gdouble) delta) <= gtk_adjustment_get_upper (adj) - gtk_adjustment_get_page_size (adj)) {
		gtk_adjustment_set_value(adj,
			gtk_adjustment_get_value (adj) + (gdouble) delta);
		nav->priv->scroll_pos++;
	} else {
		if (delta > 0)
		      gtk_adjustment_set_value (adj,
		      	gtk_adjustment_get_upper (adj) - gtk_adjustment_get_page_size (adj));
		else
		      gtk_adjustment_set_value (adj, 0);

		nav->priv->scroll_pos = 0;

		return FALSE;
	}

	return TRUE;
}

static void
eom_thumb_nav_button_clicked (GtkButton *button, EomThumbNav *nav)
{
	nav->priv->scroll_pos = 0;

	nav->priv->scroll_dir = gtk_widget_get_direction (GTK_WIDGET (button)) == GTK_TEXT_DIR_LTR ?
		GTK_WIDGET (button) == nav->priv->button_right :
		GTK_WIDGET (button) == nav->priv->button_left;

	eom_thumb_nav_scroll_step (nav);
}

static void
eom_thumb_nav_start_scroll (GtkButton *button, EomThumbNav *nav)
{
	nav->priv->scroll_dir = gtk_widget_get_direction (GTK_WIDGET (button)) == GTK_TEXT_DIR_LTR ?
		GTK_WIDGET (button) == nav->priv->button_right :
		GTK_WIDGET (button) == nav->priv->button_left;

	nav->priv->scroll_id = g_timeout_add (EOM_THUMB_NAV_SCROLL_TIMEOUT,
					      eom_thumb_nav_scroll_step,
					      nav);
}

static void
eom_thumb_nav_stop_scroll (GtkButton *button, EomThumbNav *nav)
{
	if (nav->priv->scroll_id > 0) {
		g_source_remove (nav->priv->scroll_id);
		nav->priv->scroll_id = 0;
		nav->priv->scroll_pos = 0;
	}
}

static void
eom_thumb_nav_get_property (GObject    *object,
			    guint       property_id,
			    GValue     *value,
			    GParamSpec *pspec)
{
	EomThumbNav *nav = EOM_THUMB_NAV (object);

	switch (property_id)
	{
	case PROP_SHOW_BUTTONS:
		g_value_set_boolean (value,
			eom_thumb_nav_get_show_buttons (nav));
		break;

	case PROP_THUMB_VIEW:
		g_value_set_object (value, nav->priv->thumbview);
		break;

	case PROP_MODE:
		g_value_set_int (value,
			eom_thumb_nav_get_mode (nav));
		break;
	}
}

static void
eom_thumb_nav_set_property (GObject      *object,
			    guint         property_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
	EomThumbNav *nav = EOM_THUMB_NAV (object);

	switch (property_id)
	{
	case PROP_SHOW_BUTTONS:
		eom_thumb_nav_set_show_buttons (nav,
			g_value_get_boolean (value));
		break;

	case PROP_THUMB_VIEW:
		nav->priv->thumbview =
			GTK_WIDGET (g_value_get_object (value));
		break;

	case PROP_MODE:
		eom_thumb_nav_set_mode (nav,
			g_value_get_int (value));
		break;
	}
}

static GObject *
eom_thumb_nav_constructor (GType type,
			   guint n_construct_properties,
			   GObjectConstructParam *construct_params)
{
	GObject *object;
	EomThumbNavPrivate *priv;

	object = G_OBJECT_CLASS (eom_thumb_nav_parent_class)->constructor
			(type, n_construct_properties, construct_params);

	priv = EOM_THUMB_NAV (object)->priv;

	if (priv->thumbview != NULL) {
		gtk_container_add (GTK_CONTAINER (priv->sw), priv->thumbview);
		gtk_widget_show_all (priv->sw);
	}

	return object;
}

static void
eom_thumb_nav_class_init (EomThumbNavClass *class)
{
	GObjectClass *g_object_class = (GObjectClass *) class;

	g_object_class->constructor  = eom_thumb_nav_constructor;
	g_object_class->get_property = eom_thumb_nav_get_property;
	g_object_class->set_property = eom_thumb_nav_set_property;

	g_object_class_install_property (g_object_class,
	                                 PROP_SHOW_BUTTONS,
	                                 g_param_spec_boolean ("show-buttons",
	                                                       "Show Buttons",
	                                                       "Whether to show navigation buttons or not",
	                                                       TRUE,
	                                                       (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_object_class_install_property (g_object_class,
	                                 PROP_THUMB_VIEW,
	                                 g_param_spec_object ("thumbview",
	                                                       "Thumbnail View",
	                                                       "The internal thumbnail viewer widget",
	                                                       EOM_TYPE_THUMB_VIEW,
	                                                       (G_PARAM_CONSTRUCT_ONLY |
								G_PARAM_READABLE |
								G_PARAM_WRITABLE)));

	g_object_class_install_property (g_object_class,
	                                 PROP_MODE,
	                                 g_param_spec_int ("mode",
	                                                   "Mode",
	                                                   "Thumb navigator mode",
	                                                   EOM_THUMB_NAV_MODE_ONE_ROW,
							   EOM_THUMB_NAV_MODE_MULTIPLE_ROWS,
							   EOM_THUMB_NAV_MODE_ONE_ROW,
	                                                   (G_PARAM_READABLE | G_PARAM_WRITABLE)));

	g_type_class_add_private (g_object_class, sizeof (EomThumbNavPrivate));
}

static void
eom_thumb_nav_init (EomThumbNav *nav)
{
	EomThumbNavPrivate *priv;
	GtkWidget *arrow;

	nav->priv = EOM_THUMB_NAV_GET_PRIVATE (nav);

	priv = nav->priv;

	priv->mode = EOM_THUMB_NAV_MODE_ONE_ROW;

	priv->show_buttons = TRUE;

        priv->button_left = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (priv->button_left), GTK_RELIEF_NONE);

	arrow = gtk_arrow_new (GTK_ARROW_LEFT, GTK_SHADOW_ETCHED_IN);
	gtk_container_add (GTK_CONTAINER (priv->button_left), arrow);

	gtk_widget_set_size_request (GTK_WIDGET (priv->button_left), 25, 0);

        gtk_box_pack_start (GTK_BOX (nav), priv->button_left, FALSE, FALSE, 0);

	g_signal_connect (priv->button_left,
			  "clicked",
			  G_CALLBACK (eom_thumb_nav_button_clicked),
			  nav);

	g_signal_connect (priv->button_left,
			  "pressed",
			  G_CALLBACK (eom_thumb_nav_start_scroll),
			  nav);

	g_signal_connect (priv->button_left,
			  "released",
			  G_CALLBACK (eom_thumb_nav_stop_scroll),
			  nav);

	priv->sw = gtk_scrolled_window_new (NULL, NULL);

	gtk_widget_set_name (gtk_scrolled_window_get_hscrollbar (GTK_SCROLLED_WINDOW (priv->sw)), "eom-image-collection-scrollbar");

	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (priv->sw),
					     GTK_SHADOW_IN);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_NEVER);

	g_signal_connect (priv->sw,
			  "scroll-event",
			  G_CALLBACK (eom_thumb_nav_scroll_event),
			  nav);

	priv->adj = gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (priv->sw));

	g_signal_connect (priv->adj,
			  "changed",
			  G_CALLBACK (eom_thumb_nav_adj_changed),
			  nav);

	g_signal_connect (priv->adj,
			  "value-changed",
			  G_CALLBACK (eom_thumb_nav_adj_value_changed),
			  nav);

        gtk_box_pack_start (GTK_BOX (nav), priv->sw, TRUE, TRUE, 0);

        priv->button_right = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (priv->button_right), GTK_RELIEF_NONE);

	arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_NONE);
	gtk_container_add (GTK_CONTAINER (priv->button_right), arrow);

	gtk_widget_set_size_request (GTK_WIDGET (priv->button_right), 25, 0);

        gtk_box_pack_start (GTK_BOX (nav), priv->button_right, FALSE, FALSE, 0);

	g_signal_connect (priv->button_right,
			  "clicked",
			  G_CALLBACK (eom_thumb_nav_button_clicked),
			  nav);

	g_signal_connect (priv->button_right,
			  "pressed",
			  G_CALLBACK (eom_thumb_nav_start_scroll),
			  nav);

	g_signal_connect (priv->button_right,
			  "released",
			  G_CALLBACK (eom_thumb_nav_stop_scroll),
			  nav);

	gtk_adjustment_value_changed (priv->adj);
}

/**
 * eom_thumb_nav_new:
 * @thumbview: an #EomThumbView to embed in the navigation widget.
 * @mode: The navigation mode.
 * @show_buttons: Whether to show the navigation buttons.
 *
 * Creates a new thumbnail navigation widget.
 *
 * Returns: a new #EomThumbNav object.
 **/
GtkWidget *
eom_thumb_nav_new (GtkWidget       *thumbview,
		   EomThumbNavMode  mode,
		   gboolean         show_buttons)
{
	GObject *nav;

	nav = g_object_new (EOM_TYPE_THUMB_NAV,
		            "show-buttons", show_buttons,
		            "mode", mode,
		            "thumbview", thumbview,
		            "homogeneous", FALSE,
		            "spacing", 0,
			    NULL);

	return GTK_WIDGET (nav);
}

/**
 * eom_thumb_nav_get_show_buttons:
 * @nav: an #EomThumbNav.
 *
 * Gets whether the navigation buttons are visible.
 *
 * Returns: %TRUE if the navigation buttons are visible,
 * %FALSE otherwise.
 **/
gboolean
eom_thumb_nav_get_show_buttons (EomThumbNav *nav)
{
	g_return_val_if_fail (EOM_IS_THUMB_NAV (nav), FALSE);

	return nav->priv->show_buttons;
}

/**
 * eom_thumb_nav_set_show_buttons:
 * @nav: an #EomThumbNav.
 * @show_buttons: %TRUE to show the buttons, %FALSE to hide them.
 *
 * Sets whether the navigation buttons to the left and right of the
 * widget should be visible.
 **/
void
eom_thumb_nav_set_show_buttons (EomThumbNav *nav, gboolean show_buttons)
{
	g_return_if_fail (EOM_IS_THUMB_NAV (nav));
	g_return_if_fail (nav->priv->button_left  != NULL);
	g_return_if_fail (nav->priv->button_right != NULL);

	nav->priv->show_buttons = show_buttons;

	if (show_buttons &&
	    nav->priv->mode == EOM_THUMB_NAV_MODE_ONE_ROW) {
		gtk_widget_show_all (nav->priv->button_left);
		gtk_widget_show_all (nav->priv->button_right);
	} else {
		gtk_widget_hide_all (nav->priv->button_left);
		gtk_widget_hide_all (nav->priv->button_right);
	}
}

/**
 * eom_thumb_nav_get_mode:
 * @nav: an #EomThumbNav.
 *
 * Gets the navigation mode in @nav.
 *
 * Returns: A value in #EomThumbNavMode.
 **/
EomThumbNavMode
eom_thumb_nav_get_mode (EomThumbNav *nav)
{
	g_return_val_if_fail (EOM_IS_THUMB_NAV (nav), FALSE);

	return nav->priv->mode;
}

/**
 * eom_thumb_nav_set_mode:
 * @nav: An #EomThumbNav.
 * @mode: One of #EomThumbNavMode.
 *
 * Sets the navigation mode in @nav. See #EomThumbNavMode for details.
 **/
void
eom_thumb_nav_set_mode (EomThumbNav *nav, EomThumbNavMode mode)
{
	EomThumbNavPrivate *priv;

	g_return_if_fail (EOM_IS_THUMB_NAV (nav));

	priv = nav->priv;

	priv->mode = mode;

	switch (mode)
	{
	case EOM_THUMB_NAV_MODE_ONE_ROW:
		gtk_icon_view_set_columns (GTK_ICON_VIEW (priv->thumbview),
					   G_MAXINT);

		gtk_widget_set_size_request (priv->thumbview, -1, -1);
		eom_thumb_view_set_item_height (EOM_THUMB_VIEW (priv->thumbview),
						115);

		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_NEVER);

		eom_thumb_nav_set_show_buttons (nav, priv->show_buttons);

		break;

	case EOM_THUMB_NAV_MODE_ONE_COLUMN:
		gtk_icon_view_set_columns (GTK_ICON_VIEW (priv->thumbview), 1);

		gtk_widget_set_size_request (priv->thumbview, -1, -1);
		eom_thumb_view_set_item_height (EOM_THUMB_VIEW (priv->thumbview),
						-1);

		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
						GTK_POLICY_NEVER,
						GTK_POLICY_AUTOMATIC);

		gtk_widget_hide_all (priv->button_left);
		gtk_widget_hide_all (priv->button_right);

		break;

	case EOM_THUMB_NAV_MODE_MULTIPLE_ROWS:
		gtk_icon_view_set_columns (GTK_ICON_VIEW (priv->thumbview), -1);

		gtk_widget_set_size_request (priv->thumbview, -1, 220);
		eom_thumb_view_set_item_height (EOM_THUMB_VIEW (priv->thumbview),
						-1);

		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
						GTK_POLICY_NEVER,
						GTK_POLICY_AUTOMATIC);

		gtk_widget_hide_all (priv->button_left);
		gtk_widget_hide_all (priv->button_right);

		break;

	case EOM_THUMB_NAV_MODE_MULTIPLE_COLUMNS:
		gtk_icon_view_set_columns (GTK_ICON_VIEW (priv->thumbview), -1);

		gtk_widget_set_size_request (priv->thumbview, 230, -1);
		eom_thumb_view_set_item_height (EOM_THUMB_VIEW (priv->thumbview),
						-1);

		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->sw),
						GTK_POLICY_NEVER,
						GTK_POLICY_AUTOMATIC);

		gtk_widget_hide_all (priv->button_left);
		gtk_widget_hide_all (priv->button_right);

		break;
	}
}
