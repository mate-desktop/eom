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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_METADATA_DETAILS__
#define __EOM_METADATA_DETAILS__

#include <glib-object.h>
#include <gtk/gtk.h>
#if HAVE_EXIF
#include <libexif/exif-data.h>
#endif
#if HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

G_BEGIN_DECLS

typedef struct _EomMetadataDetails EomMetadataDetails;
typedef struct _EomMetadataDetailsClass EomMetadataDetailsClass;
typedef struct _EomMetadataDetailsPrivate EomMetadataDetailsPrivate;

#define EOM_TYPE_METADATA_DETAILS            (eom_metadata_details_get_type ())
#define EOM_METADATA_DETAILS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_METADATA_DETAILS, EomMetadataDetails))
#define EOM_METADATA_DETAILS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), EOM_TYPE_METADATA_DETAILS, EomMetadataDetailsClass))
#define EOM_IS_METADATA_DETAILS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_METADATA_DETAILS))
#define EOM_IS_METADATA_DETAILS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_METADATA_DETAILS))
#define EOM_METADATA_DETAILS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EOM_TYPE_METADATA_DETAILS, EomMetadataDetailsClass))

struct _EomMetadataDetails {
        GtkTreeView parent;

	EomMetadataDetailsPrivate *priv;
};

struct _EomMetadataDetailsClass {
	GtkTreeViewClass parent_class;
};

G_GNUC_INTERNAL
GType               eom_metadata_details_get_type    (void) G_GNUC_CONST;

G_GNUC_INTERNAL
GtkWidget          *eom_metadata_details_new         (void);

#if HAVE_EXIF
G_GNUC_INTERNAL
void                eom_metadata_details_update      (EomMetadataDetails *details,
                                                      ExifData       *data);
#endif
#if HAVE_EXEMPI
G_GNUC_INTERNAL
void                eom_metadata_details_xmp_update  (EomMetadataDetails *details,
                                                      XmpPtr          xmp_data);
#endif

G_END_DECLS

#endif /* __EOM_METADATA_DETAILS__ */
