/* Eye Of MATE -- PNG Metadata Reader
 *
 * Copyright (C) 2008 The Free Software Foundation
 *
 * Author: Felix Riemann <friemann@svn.gnome.org>
 *
 * Based on the old EomMetadataReader code.
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

#ifndef _EOM_METADATA_READER_PNG_H_
#define _EOM_METADATA_READER_PNG_H_

G_BEGIN_DECLS

#define EOM_TYPE_METADATA_READER_PNG		(eom_metadata_reader_png_get_type ())
#define EOM_METADATA_READER_PNG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_METADATA_READER_PNG, EomMetadataReaderPng))
#define EOM_METADATA_READER_PNG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_METADATA_READER_PNG, EomMetadataReaderPngClass))
#define EOM_IS_METADATA_READER_PNG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_METADATA_READER_PNG))
#define EOM_IS_METADATA_READER_PNG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_METADATA_READER_PNG))
#define EOM_METADATA_READER_PNG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_METADATA_READER_PNG, EomMetadataReaderPngClass))

typedef struct _EomMetadataReaderPng EomMetadataReaderPng;
typedef struct _EomMetadataReaderPngClass EomMetadataReaderPngClass;
typedef struct _EomMetadataReaderPngPrivate EomMetadataReaderPngPrivate;

struct _EomMetadataReaderPng {
	GObject parent;

	EomMetadataReaderPngPrivate *priv;
};

struct _EomMetadataReaderPngClass {
	GObjectClass parent_klass;
};

G_GNUC_INTERNAL
GType		      eom_metadata_reader_png_get_type	(void) G_GNUC_CONST;

G_END_DECLS

#endif /* _EOM_METADATA_READER_PNG_H_ */
