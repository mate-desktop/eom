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

#ifndef __EOM_STATUSBAR_H__
#define __EOM_STATUSBAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _EomStatusbar        EomStatusbar;
typedef struct _EomStatusbarPrivate EomStatusbarPrivate;
typedef struct _EomStatusbarClass   EomStatusbarClass;

#define EOM_TYPE_STATUSBAR            (eom_statusbar_get_type ())
#define EOM_STATUSBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_STATUSBAR, EomStatusbar))
#define EOM_STATUSBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),   EOM_TYPE_STATUSBAR, EomStatusbarClass))
#define EOM_IS_STATUSBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_STATUSBAR))
#define EOM_IS_STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  EOM_TYPE_STATUSBAR))
#define EOM_STATUSBAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  EOM_TYPE_STATUSBAR, EomStatusbarClass))

struct _EomStatusbar
{
        GtkStatusbar parent;

        EomStatusbarPrivate *priv;
};

struct _EomStatusbarClass
{
        GtkStatusbarClass parent_class;
};

GType		 eom_statusbar_get_type			(void) G_GNUC_CONST;

GtkWidget	*eom_statusbar_new			(void);

void		 eom_statusbar_set_image_number		(EomStatusbar   *statusbar,
							 gint           num,
							 gint           tot);

void		 eom_statusbar_set_progress		(EomStatusbar   *statusbar,
							 gdouble        progress);

G_END_DECLS

#endif /* __EOM_STATUSBAR_H__ */
