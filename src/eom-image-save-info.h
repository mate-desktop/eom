#ifndef _EOM_IMAGE_SAVE_INFO_H_
#define _EOM_IMAGE_SAVE_INFO_H_

#include <glib-object.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#ifndef __EOM_IMAGE_DECLR__
#define __EOM_IMAGE_DECLR__
typedef struct _EomImage EomImage;
#endif

#define EOM_TYPE_IMAGE_SAVE_INFO            (eom_image_save_info_get_type ())
#define EOM_IMAGE_SAVE_INFO(o)         (G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_IMAGE_SAVE_INFO, EomImageSaveInfo))
#define EOM_IMAGE_SAVE_INFO_CLASS(k)   (G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_IMAGE_SAVE_INFO, EomImageSaveInfoClass))
#define EOM_IS_IMAGE_SAVE_INFO(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_IMAGE_SAVE_INFO))
#define EOM_IS_IMAGE_SAVE_INFO_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_IMAGE_SAVE_INFO))
#define EOM_IMAGE_SAVE_INFO_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_IMAGE_SAVE_INFO, EomImageSaveInfoClass))

typedef struct _EomImageSaveInfo EomImageSaveInfo;
typedef struct _EomImageSaveInfoClass EomImageSaveInfoClass;

struct _EomImageSaveInfo {
	GObject parent;

	GFile       *file;
	char        *format;
	gboolean     exists;
	gboolean     local;
	gboolean     has_metadata;
	gboolean     modified;
	gboolean     overwrite;

	float        jpeg_quality; /* valid range: [0.0 ... 1.0] */
};

struct _EomImageSaveInfoClass {
	GObjectClass parent_klass;
};

#define EOM_FILE_FORMAT_JPEG   "jpeg"

GType             eom_image_save_info_get_type         (void) G_GNUC_CONST;

EomImageSaveInfo *eom_image_save_info_new_from_image   (EomImage        *image);

EomImageSaveInfo *eom_image_save_info_new_from_uri     (const char      *uri,
						       GdkPixbufFormat  *format);

EomImageSaveInfo *eom_image_save_info_new_from_file    (GFile           *file,
						       GdkPixbufFormat  *format);

G_END_DECLS

#endif /* _EOM_IMAGE_SAVE_INFO_H_ */
