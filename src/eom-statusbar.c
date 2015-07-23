/* Eye of Mate - Statusbar
 *
 * Copyright (C) 2000-2006 The Free Software Foundation
 *
 * Author: Federico Mena-Quintero <federico@gnu.org>
 *	   Jens Finke <jens@gnome.org>
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

#include "eom-statusbar.h"

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define EOM_STATUSBAR_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EOM_TYPE_STATUSBAR, EomStatusbarPrivate))

#if GTK_CHECK_VERSION (3, 2, 0)
#define gtk_hbox_new(X,Y) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,Y)
#define gtk_vbox_new(X,Y) gtk_box_new(GTK_ORIENTATION_VERTICAL,Y)
#endif

G_DEFINE_TYPE (EomStatusbar, eom_statusbar, GTK_TYPE_STATUSBAR)

struct _EomStatusbarPrivate
{
	GtkWidget *progressbar;
	GtkWidget *img_num_label;
};

static void
eom_statusbar_class_init (EomStatusbarClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (g_object_class, sizeof (EomStatusbarPrivate));
}

static void
eom_statusbar_init (EomStatusbar *statusbar)
{
	EomStatusbarPrivate *priv;
	GtkWidget *vbox;

	statusbar->priv = EOM_STATUSBAR_GET_PRIVATE (statusbar);
	priv = statusbar->priv;

#if GTK_CHECK_VERSION (3, 0, 0)
	gtk_widget_set_margin_top (GTK_WIDGET (statusbar), 0);
	gtk_widget_set_margin_bottom (GTK_WIDGET (statusbar), 0);
#endif

	priv->img_num_label = gtk_label_new (NULL);
	gtk_widget_set_size_request (priv->img_num_label, 100, 10);
	gtk_widget_show (priv->img_num_label);

	gtk_box_pack_end (GTK_BOX (statusbar),
			  priv->img_num_label,
			  FALSE,
			  TRUE,
			  0);

	vbox = gtk_vbox_new (FALSE, 0);

	gtk_box_pack_end (GTK_BOX (statusbar),
			  vbox,
			  FALSE,
			  FALSE,
			  2);

	statusbar->priv->progressbar = gtk_progress_bar_new ();

	gtk_box_pack_end (GTK_BOX (vbox),
			  priv->progressbar,
			  TRUE,
			  TRUE,
			  2);

	gtk_widget_set_size_request (priv->progressbar, -1, 10);

	gtk_widget_show (vbox);

	gtk_widget_hide (statusbar->priv->progressbar);

}

GtkWidget *
eom_statusbar_new (void)
{
	return GTK_WIDGET (g_object_new (EOM_TYPE_STATUSBAR, NULL));
}

void
eom_statusbar_set_image_number (EomStatusbar *statusbar,
                                gint          num,
				gint          tot)
{
	gchar *msg;

	g_return_if_fail (EOM_IS_STATUSBAR (statusbar));

	/* Hide number display if values don't make sense */
	if (G_UNLIKELY (num <= 0 || tot <= 0))
		return;

	/* Translators: This string is displayed in the statusbar.
	 * The first token is the image number, the second is total image
	 * count.
	 *
	 * Translate to "%Id" if you want to use localized digits, or
	 * translate to "%d" otherwise.
	 *
	 * Note that translating this doesn't guarantee that you get localized
	 * digits. That needs support from your system and locale definition
	 * too.*/
	msg = g_strdup_printf (_("%d / %d"), num, tot);

	gtk_label_set_text (GTK_LABEL (statusbar->priv->img_num_label), msg);

      	g_free (msg);
}

void
eom_statusbar_set_progress (EomStatusbar *statusbar,
			    gdouble       progress)
{
	g_return_if_fail (EOM_IS_STATUSBAR (statusbar));

	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (statusbar->priv->progressbar),
				       progress);

	if (progress > 0 && progress < 1) {
		gtk_widget_show (statusbar->priv->progressbar);
		gtk_widget_hide (statusbar->priv->img_num_label);
	} else {
		gtk_widget_hide (statusbar->priv->progressbar);
		gtk_widget_show (statusbar->priv->img_num_label);
	}
}

