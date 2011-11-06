/* Eye Of MATE -- Metadata Reader Interface
 *
 * Copyright (C) 2008 The Free Software Foundation
 *
 * Author: Felix Riemann <friemann@svn.gnome.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-metadata-reader.h"
#include "eom-metadata-reader-jpg.h"
#include "eom-metadata-reader-png.h"
#include "eom-debug.h"


GType
eom_metadata_reader_get_type (void)
{
	static GType reader_type = 0;

	if (G_UNLIKELY (reader_type == 0)) {
		reader_type = g_type_register_static_simple (G_TYPE_INTERFACE,
							     "EomMetadataReader",
							     sizeof (EomMetadataReaderInterface),
							     NULL, 0, NULL, 0);
	}

	return reader_type;
}

EomMetadataReader*
eom_metadata_reader_new (EomMetadataFileType type)
{
	EomMetadataReader *emr;

	switch (type) {
	case EOM_METADATA_JPEG:
		emr = EOM_METADATA_READER (g_object_new (EOM_TYPE_METADATA_READER_JPG, NULL));
		break;
	case EOM_METADATA_PNG:
		emr = EOM_METADATA_READER (g_object_new (EOM_TYPE_METADATA_READER_PNG, NULL));
		break;
	default:
		emr = NULL;
		break;
	}

	return emr;
}

gboolean
eom_metadata_reader_finished (EomMetadataReader *emr)
{
	g_return_val_if_fail (EOM_IS_METADATA_READER (emr), TRUE);

	return EOM_METADATA_READER_GET_INTERFACE (emr)->finished (emr);
}


void
eom_metadata_reader_consume (EomMetadataReader *emr, const guchar *buf, guint len)
{
	EOM_METADATA_READER_GET_INTERFACE (emr)->consume (emr, buf, len);
}

/* Returns the raw exif data. NOTE: The caller of this function becomes
 * the new owner of this piece of memory and is responsible for freeing it!
 */
void
eom_metadata_reader_get_exif_chunk (EomMetadataReader *emr, guchar **data, guint *len)
{
	EomMetadataReaderInterface *iface;

	g_return_if_fail (data != NULL && len != NULL);
	iface = EOM_METADATA_READER_GET_INTERFACE (emr);

	if (iface->get_raw_exif) {
		iface->get_raw_exif (emr, data, len);
	} else {
		g_return_if_fail (data != NULL && len != NULL);

		*data = NULL;
		*len = 0;
	}

}

#ifdef HAVE_EXIF
ExifData*
eom_metadata_reader_get_exif_data (EomMetadataReader *emr)
{
	gpointer exif_data = NULL;
	EomMetadataReaderInterface *iface;

	iface = EOM_METADATA_READER_GET_INTERFACE (emr);

	if (iface->get_exif_data)
		exif_data = iface->get_exif_data (emr);

	return exif_data;
}
#endif

#ifdef HAVE_EXEMPI
XmpPtr
eom_metadata_reader_get_xmp_data (EomMetadataReader *emr )
{
	gpointer xmp_data = NULL;
	EomMetadataReaderInterface *iface;

	iface = EOM_METADATA_READER_GET_INTERFACE (emr);

	if (iface->get_xmp_ptr)
		xmp_data = iface->get_xmp_ptr (emr);

	return xmp_data;
}
#endif

#ifdef HAVE_LCMS
cmsHPROFILE
eom_metadata_reader_get_icc_profile (EomMetadataReader *emr)
{
	EomMetadataReaderInterface *iface;
	gpointer profile = NULL;

	iface = EOM_METADATA_READER_GET_INTERFACE (emr);

	if (iface->get_icc_profile)
		profile = iface->get_icc_profile (emr);

	return profile;
}
#endif
