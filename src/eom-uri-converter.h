#ifndef _EOM_URI_CONVERTER_H_
#define _EOM_URI_CONVERTER_H_

#include <glib-object.h>
#include <glib/gi18n.h>
#include "eom-image.h"

G_BEGIN_DECLS

#define EOM_TYPE_URI_CONVERTER          (eom_uri_converter_get_type ())
#define EOM_URI_CONVERTER(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_URI_CONVERTER, EomURIConverter))
#define EOM_URI_CONVERTER_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_URI_CONVERTER, EomURIConverterClass))
#define EOM_IS_URI_CONVERTER(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_URI_CONVERTER))
#define EOM_IS_URI_CONVERTER_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_URI_CONVERTER))
#define EOM_URI_CONVERTER_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_URI_CONVERTER, EomURIConverterClass))

#ifndef __EOM_URI_CONVERTER_DECLR__
#define __EOM_URI_CONVERTER_DECLR__
typedef struct _EomURIConverter EomURIConverter;
#endif
typedef struct _EomURIConverterClass EomURIConverterClass;
typedef struct _EomURIConverterPrivate EomURIConverterPrivate;

typedef enum {
	EOM_UC_STRING,
	EOM_UC_FILENAME,
	EOM_UC_COUNTER,
	EOM_UC_COMMENT,
	EOM_UC_DATE,
	EOM_UC_TIME,
	EOM_UC_DAY,
	EOM_UC_MONTH,
	EOM_UC_YEAR,
	EOM_UC_HOUR,
	EOM_UC_MINUTE,
	EOM_UC_SECOND,
	EOM_UC_END
} EomUCType;

typedef struct {
	char     *description;
	char     *rep;
	gboolean req_exif;
} EomUCInfo;

typedef enum {
	EOM_UC_ERROR_INVALID_UNICODE,
	EOM_UC_ERROR_INVALID_CHARACTER,
	EOM_UC_ERROR_EQUAL_FILENAMES,
	EOM_UC_ERROR_UNKNOWN
} EomUCError;

#define EOM_UC_ERROR eom_uc_error_quark ()


struct _EomURIConverter {
	GObject parent;

	EomURIConverterPrivate *priv;
};

struct _EomURIConverterClass {
	GObjectClass parent_klass;
};

G_GNUC_INTERNAL
GType              eom_uri_converter_get_type      (void) G_GNUC_CONST;

G_GNUC_INTERNAL
GQuark             eom_uc_error_quark              (void);

G_GNUC_INTERNAL
EomURIConverter*   eom_uri_converter_new           (GFile *base_file,
                                                    GdkPixbufFormat *img_format,
						    const char *format_string);

G_GNUC_INTERNAL
gboolean           eom_uri_converter_check         (EomURIConverter *converter,
                                                    GList *img_list,
                                                    GError **error);

G_GNUC_INTERNAL
gboolean           eom_uri_converter_requires_exif (EomURIConverter *converter);

G_GNUC_INTERNAL
gboolean           eom_uri_converter_do            (EomURIConverter *converter,
                                                    EomImage *image,
                                                    GFile **file,
                                                    GdkPixbufFormat **format,
                                                    GError **error);

G_GNUC_INTERNAL
char*              eom_uri_converter_preview       (const char *format_str,
                                                    EomImage *img,
                                                    GdkPixbufFormat *format,
						    gulong counter,
						    guint n_images,
						    gboolean convert_spaces,
						    gunichar space_char);

/* for debugging purpose only */
G_GNUC_INTERNAL
void                eom_uri_converter_print_list (EomURIConverter *conv);

G_END_DECLS

#endif /* _EOM_URI_CONVERTER_H_ */
