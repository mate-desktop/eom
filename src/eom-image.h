/* Eye Of Mate - Image
 *
 * Copyright (C) 2007 The Free Software Foundation
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

#ifndef __EOM_IMAGE_H__
#define __EOM_IMAGE_H__

#include "eom-jobs.h"
#include "eom-window.h"
#include "eom-transform.h"
#include "eom-image-save-info.h"
#include "eom-enums.h"

#include <glib.h>
#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#ifdef HAVE_EXIF
#include <libexif/exif-data.h>
#include "eom-exif-util.h"
#endif

#ifdef HAVE_LCMS
#include <lcms2.h>
#endif

#ifdef HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

#ifdef HAVE_RSVG
#include <librsvg/rsvg.h>
#endif

G_BEGIN_DECLS

#ifndef __EOM_IMAGE_DECLR__
#define __EOM_IMAGE_DECLR__
typedef struct _EomImage EomImage;
#endif
typedef struct _EomImageClass EomImageClass;
typedef struct _EomImagePrivate EomImagePrivate;

#define EOM_TYPE_IMAGE            (eom_image_get_type ())
#define EOM_IMAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_IMAGE, EomImage))
#define EOM_IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_IMAGE, EomImageClass))
#define EOM_IS_IMAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_IMAGE))
#define EOM_IS_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  EOM_TYPE_IMAGE))
#define EOM_IMAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_IMAGE, EomImageClass))

typedef enum {
	EOM_IMAGE_ERROR_SAVE_NOT_LOCAL,
	EOM_IMAGE_ERROR_NOT_LOADED,
	EOM_IMAGE_ERROR_VFS,
	EOM_IMAGE_ERROR_FILE_EXISTS,
	EOM_IMAGE_ERROR_TMP_FILE_FAILED,
	EOM_IMAGE_ERROR_GENERIC,
	EOM_IMAGE_ERROR_UNKNOWN
} EomImageError;

#define EOM_IMAGE_ERROR eom_image_error_quark ()

typedef enum {
	EOM_IMAGE_STATUS_UNKNOWN,
	EOM_IMAGE_STATUS_LOADING,
	EOM_IMAGE_STATUS_LOADED,
	EOM_IMAGE_STATUS_SAVING,
	EOM_IMAGE_STATUS_FAILED
} EomImageStatus;

typedef enum {
  EOM_IMAGE_METADATA_NOT_READ,
  EOM_IMAGE_METADATA_NOT_AVAILABLE,
  EOM_IMAGE_METADATA_READY
} EomImageMetadataStatus;

struct _EomImage {
	GObject parent;

	EomImagePrivate *priv;
};

struct _EomImageClass {
	GObjectClass parent_class;

	void (* changed) 	   (EomImage *img);

	void (* size_prepared)     (EomImage *img,
				    int       width,
				    int       height);

	void (* thumbnail_changed) (EomImage *img);

	void (* save_progress)     (EomImage *img,
				    gfloat    progress);

	void (* next_frame)        (EomImage *img,
				    gint delay);

	void (* file_changed)      (EomImage *img);
};

GType	          eom_image_get_type	             (void) G_GNUC_CONST;

GQuark            eom_image_error_quark              (void);

EomImage         *eom_image_new_file                 (GFile *file, const gchar *caption);

gboolean          eom_image_load                     (EomImage   *img,
					              EomImageData data2read,
					              EomJob     *job,
					              GError    **error);

void              eom_image_cancel_load              (EomImage   *img);

gboolean          eom_image_has_data                 (EomImage   *img,
					              EomImageData data);

void              eom_image_data_ref                 (EomImage   *img);

void              eom_image_data_unref               (EomImage   *img);

void              eom_image_set_thumbnail            (EomImage   *img,
					              GdkPixbuf  *pixbuf);

gboolean          eom_image_save_as_by_info          (EomImage   *img,
		      			              EomImageSaveInfo *source,
		      			              EomImageSaveInfo *target,
		      			              GError    **error);

gboolean          eom_image_save_by_info             (EomImage   *img,
					              EomImageSaveInfo *source,
					              GError    **error);

GdkPixbuf*        eom_image_get_pixbuf               (EomImage   *img);

GdkPixbuf*        eom_image_get_thumbnail            (EomImage   *img);

void              eom_image_get_size                 (EomImage   *img,
					              gint       *width,
					              gint       *height);

goffset           eom_image_get_bytes                (EomImage   *img);

gboolean          eom_image_is_modified              (EomImage   *img);

void              eom_image_modified                 (EomImage   *img);

const gchar*      eom_image_get_caption              (EomImage   *img);

const gchar      *eom_image_get_collate_key          (EomImage   *img);

#ifdef HAVE_EXIF
ExifData*         eom_image_get_exif_info            (EomImage   *img);
#endif

gpointer          eom_image_get_xmp_info             (EomImage   *img);

GFile*            eom_image_get_file                 (EomImage   *img);

gchar*            eom_image_get_uri_for_display      (EomImage   *img);

EomImageStatus    eom_image_get_status               (EomImage   *img);

EomImageMetadataStatus eom_image_get_metadata_status (EomImage   *img);

void              eom_image_transform                (EomImage   *img,
						      EomTransform *trans,
						      EomJob     *job);

void              eom_image_autorotate               (EomImage   *img);

#ifdef HAVE_LCMS
cmsHPROFILE       eom_image_get_profile              (EomImage    *img);

void              eom_image_apply_display_profile    (EomImage    *img,
						      cmsHPROFILE  display_profile);
#endif

void              eom_image_undo                     (EomImage   *img);

GList		 *eom_image_get_supported_mime_types (void);

gboolean          eom_image_is_supported_mime_type   (const char *mime_type);

gboolean          eom_image_is_animation             (EomImage *img);

gboolean          eom_image_start_animation          (EomImage *img);

#ifdef HAVE_RSVG
gboolean          eom_image_is_svg                   (EomImage *img);
RsvgHandle       *eom_image_get_svg                  (EomImage *img);
#endif

EomTransform     *eom_image_get_transform            (EomImage *img);
EomTransform     *eom_image_get_autorotate_transform (EomImage *img);

gboolean          eom_image_is_jpeg                  (EomImage *img);

void              eom_image_file_changed             (EomImage *img);
gboolean          eom_image_is_file_changed         (EomImage *img);

G_END_DECLS

#endif /* __EOM_IMAGE_H__ */
