/* Eye Of Mate - Thumbnail View
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Claudio Saavedra <csaavedra@alumnos.utalca.cl>
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

#ifndef EOM_THUMB_VIEW_H
#define EOM_THUMB_VIEW_H

#include "eom-image.h"
#include "eom-list-store.h"

G_BEGIN_DECLS

#define EOM_TYPE_THUMB_VIEW            (eom_thumb_view_get_type ())
#define EOM_THUMB_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_THUMB_VIEW, EomThumbView))
#define EOM_THUMB_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  EOM_TYPE_THUMB_VIEW, EomThumbViewClass))
#define EOM_IS_THUMB_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_THUMB_VIEW))
#define EOM_IS_THUMB_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  EOM_TYPE_THUMB_VIEW))
#define EOM_THUMB_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  EOM_TYPE_THUMB_VIEW, EomThumbViewClass))

typedef struct _EomThumbView EomThumbView;
typedef struct _EomThumbViewClass EomThumbViewClass;
typedef struct _EomThumbViewPrivate EomThumbViewPrivate;

typedef enum {
	EOM_THUMB_VIEW_SELECT_CURRENT = 0,
	EOM_THUMB_VIEW_SELECT_LEFT,
	EOM_THUMB_VIEW_SELECT_RIGHT,
	EOM_THUMB_VIEW_SELECT_FIRST,
	EOM_THUMB_VIEW_SELECT_LAST,
	EOM_THUMB_VIEW_SELECT_RANDOM
} EomThumbViewSelectionChange;

struct _EomThumbView {
	GtkIconView icon_view;
	EomThumbViewPrivate *priv;
};

struct _EomThumbViewClass {
	 GtkIconViewClass icon_view_class;
};

GType       eom_thumb_view_get_type 		    (void) G_GNUC_CONST;

GtkWidget  *eom_thumb_view_new 			    (void);

void	    eom_thumb_view_set_model 		    (EomThumbView *thumbview,
						     EomListStore *store);

void        eom_thumb_view_set_item_height          (EomThumbView *thumbview,
						     gint          height);

guint	    eom_thumb_view_get_n_selected 	    (EomThumbView *thumbview);

EomImage   *eom_thumb_view_get_first_selected_image (EomThumbView *thumbview);

GList      *eom_thumb_view_get_selected_images 	    (EomThumbView *thumbview);

void        eom_thumb_view_select_single 	    (EomThumbView *thumbview,
						     EomThumbViewSelectionChange change);

void        eom_thumb_view_set_current_image	    (EomThumbView *thumbview,
						     EomImage     *image,
						     gboolean     deselect_other);

void        eom_thumb_view_set_thumbnail_popup      (EomThumbView *thumbview,
						     GtkMenu      *menu);

G_END_DECLS

#endif /* EOM_THUMB_VIEW_H */
