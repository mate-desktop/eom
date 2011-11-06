/* Eye Of Mate - EOM Image Exif Details
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

#ifndef __EOM_EXIF_DETAILS__
#define __EOM_EXIF_DETAILS__

#include <glib-object.h>
#include <gtk/gtk.h>
#if HAVE_EXIF
#include <libexif/exif-data.h>
#endif
#if HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

G_BEGIN_DECLS

typedef struct _EomExifDetails EomExifDetails;
typedef struct _EomExifDetailsClass EomExifDetailsClass;
typedef struct _EomExifDetailsPrivate EomExifDetailsPrivate;

#define EOM_TYPE_EXIF_DETAILS            (eom_exif_details_get_type ())
#define EOM_EXIF_DETAILS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_EXIF_DETAILS, EomExifDetails))
#define EOM_EXIF_DETAILS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), EOM_TYPE_EXIF_DETAILS, EomExifDetailsClass))
#define EOM_IS_EXIF_DETAILS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_EXIF_DETAILS))
#define EOM_IS_EXIF_DETAILS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_EXIF_DETAILS))
#define EOM_EXIF_DETAILS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EOM_TYPE_EXIF_DETAILS, EomExifDetailsClass))

struct _EomExifDetails {
        GtkTreeView parent;

	EomExifDetailsPrivate *priv;
};

struct _EomExifDetailsClass {
	GtkTreeViewClass parent_class;
};

GType               eom_exif_details_get_type    (void) G_GNUC_CONST;

GtkWidget          *eom_exif_details_new         (void);

#if HAVE_EXIF
void                eom_exif_details_update      (EomExifDetails *view,
						  ExifData       *data);
#endif
#if HAVE_EXEMPI
void                eom_exif_details_xmp_update  (EomExifDetails *view,
							XmpPtr          xmp_data);
#endif

G_END_DECLS

#endif /* __EOM_EXIF_DETAILS__ */
