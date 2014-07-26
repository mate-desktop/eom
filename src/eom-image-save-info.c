#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include "eom-image-save-info.h"
#include "eom-image-private.h"
#include "eom-pixbuf-util.h"
#include "eom-image.h"

G_DEFINE_TYPE (EomImageSaveInfo, eom_image_save_info, G_TYPE_OBJECT)

static void
eom_image_save_info_dispose (GObject *object)
{
	EomImageSaveInfo *info = EOM_IMAGE_SAVE_INFO (object);

	if (info->file != NULL) {
		g_object_unref (info->file);
		info->file = NULL;
	}

	if (info->format != NULL) {
		g_free (info->format);
		info->format = NULL;
	}

	G_OBJECT_CLASS (eom_image_save_info_parent_class)->dispose (object);
}

static void
eom_image_save_info_init (EomImageSaveInfo *obj)
{

}

static void
eom_image_save_info_class_init (EomImageSaveInfoClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;

	object_class->dispose = eom_image_save_info_dispose;
}

/* is_local_uri:
 *
 * Checks if the URI points to a local file system. This tests simply
 * if the URI scheme is 'file'. This function is used to ensure that
 * we can write to the path-part of the URI with non-VFS aware
 * filesystem calls.
 */
static gboolean
is_local_file (GFile *file)
{
	char *scheme;
	gboolean ret;

	g_return_val_if_fail (file != NULL, FALSE);

	scheme = g_file_get_uri_scheme (file);

	ret = (g_ascii_strcasecmp (scheme, "file") == 0);
	g_free (scheme);
	return ret;
}

static char*
get_save_file_type_by_file (GFile *file)
{
	GdkPixbufFormat *format;
	char *type = NULL;

	format = eom_pixbuf_get_format (file);
	if (format != NULL) {
		type = gdk_pixbuf_format_get_name (format);
	}

	return type;
}

EomImageSaveInfo*
eom_image_save_info_new_from_image (EomImage *image)
{
	EomImageSaveInfo *info = NULL;

	g_return_val_if_fail (EOM_IS_IMAGE (image), NULL);

	info = g_object_new (EOM_TYPE_IMAGE_SAVE_INFO, NULL);

	info->file         = eom_image_get_file (image);
	info->format       = g_strdup (image->priv->file_type);
	info->exists       = g_file_query_exists (info->file, NULL);
	info->local        = is_local_file (info->file);
        info->has_metadata = eom_image_has_data (image, EOM_IMAGE_DATA_EXIF);
	info->modified     = eom_image_is_modified (image);
	info->overwrite    = FALSE;

	info->jpeg_quality = -1.0;

	return info;
}

EomImageSaveInfo*
eom_image_save_info_new_from_uri (const char *txt_uri, GdkPixbufFormat *format)
{
	GFile *file;
	EomImageSaveInfo *info;

	g_return_val_if_fail (txt_uri != NULL, NULL);

	file = g_file_new_for_uri (txt_uri);

	info = eom_image_save_info_new_from_file (file, format);

	g_object_unref (file);

	return info;
}

EomImageSaveInfo*
eom_image_save_info_new_from_file (GFile *file, GdkPixbufFormat *format)
{
	EomImageSaveInfo *info;

	g_return_val_if_fail (file != NULL, NULL);

	info = g_object_new (EOM_TYPE_IMAGE_SAVE_INFO, NULL);

	info->file = g_object_ref (file);
	if (format == NULL) {
		info->format = get_save_file_type_by_file (info->file);
	}
	else {
		info->format = gdk_pixbuf_format_get_name (format);
	}
	info->exists       = g_file_query_exists (file, NULL);
	info->local        = is_local_file (file);
        info->has_metadata = FALSE;
	info->modified     = FALSE;
	info->overwrite    = FALSE;

	info->jpeg_quality = -1.0;

	g_assert (info->format != NULL);

	return info;
}
