/* Eye Of Mate - Image Store
 *
 * Copyright (C) 2006-2007 The Free Software Foundation
 *
 * Author: Claudio Saavedra <csaavedra@alumnos.utalca.cl>
 *
 * Based on code by: Jens Finke <jens@triq.net>
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

#ifndef EOM_LIST_STORE_H
#define EOM_LIST_STORE_H

#include <gtk/gtk.h>
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#ifndef __EOM_IMAGE_DECLR__
#define __EOM_IMAGE_DECLR__
  typedef struct _EomImage EomImage;
#endif

typedef struct _EomListStore EomListStore;
typedef struct _EomListStoreClass EomListStoreClass;
typedef struct _EomListStorePrivate EomListStorePrivate;

#define EOM_TYPE_LIST_STORE            eom_list_store_get_type()
#define EOM_LIST_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_LIST_STORE, EomListStore))
#define EOM_LIST_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  EOM_TYPE_LIST_STORE, EomListStoreClass))
#define EOM_IS_LIST_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_LIST_STORE))
#define EOM_IS_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  EOM_TYPE_LIST_STORE))
#define EOM_LIST_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  EOM_TYPE_LIST_STORE, EomListStoreClass))

#define EOM_LIST_STORE_THUMB_SIZE 90

typedef enum {
	EOM_LIST_STORE_THUMBNAIL = 0,
	EOM_LIST_STORE_THUMB_SET,
	EOM_LIST_STORE_EOM_IMAGE,
	EOM_LIST_STORE_EOM_JOB,
	EOM_LIST_STORE_NUM_COLUMNS
} EomListStoreColumn;

struct _EomListStore {
        GtkListStore parent;
	EomListStorePrivate *priv;
};

struct _EomListStoreClass {
        GtkListStoreClass parent_class;

	/* Padding for future expansion */
	void (* _eom_reserved1) (void);
	void (* _eom_reserved2) (void);
	void (* _eom_reserved3) (void);
	void (* _eom_reserved4) (void);
};

GType           eom_list_store_get_type 	     (void) G_GNUC_CONST;

GtkListStore   *eom_list_store_new 		     (void);

GtkListStore   *eom_list_store_new_from_glist 	     (GList *list);

void            eom_list_store_append_image 	     (EomListStore *store,
						      EomImage     *image);

void            eom_list_store_add_files 	     (EomListStore *store,
						      GList        *file_list);

void            eom_list_store_remove_image 	     (EomListStore *store,
						      EomImage     *image);

gint            eom_list_store_get_pos_by_image      (EomListStore *store,
						      EomImage     *image);

EomImage       *eom_list_store_get_image_by_pos      (EomListStore *store,
						      gint   pos);

gint            eom_list_store_get_pos_by_iter 	     (EomListStore *store,
						      GtkTreeIter  *iter);

gint            eom_list_store_length                (EomListStore *store);

gint            eom_list_store_get_initial_pos 	     (EomListStore *store);

void            eom_list_store_thumbnail_set         (EomListStore *store,
						      GtkTreeIter *iter);

void            eom_list_store_thumbnail_unset       (EomListStore *store,
						      GtkTreeIter *iter);

void            eom_list_store_thumbnail_refresh     (EomListStore *store,
						      GtkTreeIter *iter);

G_END_DECLS

#endif
