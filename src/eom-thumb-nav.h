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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_THUMB_NAV_H__
#define __EOM_THUMB_NAV_H__

#include "eom-thumb-view.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _EomThumbNav EomThumbNav;
typedef struct _EomThumbNavClass EomThumbNavClass;
typedef struct _EomThumbNavPrivate EomThumbNavPrivate;

#define EOM_TYPE_THUMB_NAV            (eom_thumb_nav_get_type ())
#define EOM_THUMB_NAV(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_THUMB_NAV, EomThumbNav))
#define EOM_THUMB_NAV_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_THUMB_NAV, EomThumbNavClass))
#define EOM_IS_THUMB_NAV(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_THUMB_NAV))
#define EOM_IS_THUMB_NAV_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  EOM_TYPE_THUMB_NAV))
#define EOM_THUMB_NAV_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_THUMB_NAV, EomThumbNavClass))

typedef enum {
	EOM_THUMB_NAV_MODE_ONE_ROW,
	EOM_THUMB_NAV_MODE_ONE_COLUMN,
	EOM_THUMB_NAV_MODE_MULTIPLE_ROWS,
	EOM_THUMB_NAV_MODE_MULTIPLE_COLUMNS
} EomThumbNavMode;

struct _EomThumbNav {
	GtkBox base_instance;

	EomThumbNavPrivate *priv;
};

struct _EomThumbNavClass {
	GtkBoxClass parent_class;
};

GType	         eom_thumb_nav_get_type          (void) G_GNUC_CONST;

GtkWidget       *eom_thumb_nav_new               (GtkWidget         *thumbview,
						  EomThumbNavMode    mode,
	             			          gboolean           show_buttons);

gboolean         eom_thumb_nav_get_show_buttons  (EomThumbNav       *nav);

void             eom_thumb_nav_set_show_buttons  (EomThumbNav       *nav,
                                                  gboolean           show_buttons);

EomThumbNavMode  eom_thumb_nav_get_mode          (EomThumbNav       *nav);

void             eom_thumb_nav_set_mode          (EomThumbNav       *nav,
                                                  EomThumbNavMode    mode);

G_END_DECLS

#endif /* __EOM_THUMB_NAV_H__ */
