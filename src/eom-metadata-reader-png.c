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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <string.h>
#include <zlib.h>
#include <gdk/gdkx.h>

#include "eom-metadata-reader.h"
#include "eom-metadata-reader-png.h"
#include "eom-debug.h"

typedef enum {
	EMR_READ_MAGIC,
	EMR_READ_SIZE_HIGH_HIGH_BYTE,
	EMR_READ_SIZE_HIGH_LOW_BYTE,
	EMR_READ_SIZE_LOW_HIGH_BYTE,
	EMR_READ_SIZE_LOW_LOW_BYTE,
	EMR_READ_CHUNK_NAME,
	EMR_SKIP_BYTES,
	EMR_CHECK_CRC,
	EMR_SKIP_CRC,
	EMR_READ_XMP_ITXT,
	EMR_READ_ICCP,
	EMR_READ_SRGB,
	EMR_READ_CHRM,
	EMR_READ_GAMA,
	EMR_FINISHED
} EomMetadataReaderPngState;

#if 0
#define IS_FINISHED(priv) (priv->icc_chunk  != NULL && \
                           priv->xmp_chunk  != NULL)
#endif

struct _EomMetadataReaderPngPrivate {
	EomMetadataReaderPngState  state;

	/* data fields */
	guint32  icc_len;
	gpointer icc_chunk;

	gpointer xmp_chunk;
	guint32  xmp_len;

	guint32	 sRGB_len;
	gpointer sRGB_chunk;

	gpointer cHRM_chunk;
	guint32	 cHRM_len;

	guint32	 gAMA_len;
	gpointer gAMA_chunk;

	/* management fields */
	gsize      size;
	gsize      bytes_read;
	guint	   sub_step;
	guchar	   chunk_name[4];
	gpointer   *crc_chunk;
	guint32	   *crc_len;
	guint32    target_crc;
	gboolean   hasIHDR;
};

static void
eom_metadata_reader_png_init_emr_iface (gpointer g_iface, gpointer iface_data);

G_DEFINE_TYPE_WITH_CODE (EomMetadataReaderPng, eom_metadata_reader_png,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (EOM_TYPE_METADATA_READER,
			                        eom_metadata_reader_png_init_emr_iface) \
			                        G_ADD_PRIVATE(EomMetadataReaderPng))

static void
eom_metadata_reader_png_dispose (GObject *object)
{
	EomMetadataReaderPng *emr = EOM_METADATA_READER_PNG (object);
	EomMetadataReaderPngPrivate *priv = emr->priv;

	g_free (priv->xmp_chunk);
	priv->xmp_chunk = NULL;

	g_free (priv->icc_chunk);
	priv->icc_chunk = NULL;

	g_free (priv->sRGB_chunk);
	priv->sRGB_chunk = NULL;

	g_free (priv->cHRM_chunk);
	priv->cHRM_chunk = NULL;

	g_free (priv->gAMA_chunk);
	priv->gAMA_chunk = NULL;

	G_OBJECT_CLASS (eom_metadata_reader_png_parent_class)->dispose (object);
}

static void
eom_metadata_reader_png_init (EomMetadataReaderPng *emr)
{
	EomMetadataReaderPngPrivate *priv;

	priv = emr->priv =  eom_metadata_reader_png_get_instance_private (emr);
	priv->icc_chunk = NULL;
	priv->icc_len = 0;
	priv->xmp_chunk = NULL;
	priv->xmp_len = 0;
	priv->sRGB_chunk = NULL;
	priv->sRGB_len = 0;
	priv->cHRM_chunk = NULL;
	priv->cHRM_len = 0;
	priv->gAMA_chunk = NULL;
	priv->gAMA_len = 0;

	priv->sub_step = 0;
	priv->state = EMR_READ_MAGIC;
	priv->hasIHDR = FALSE;
}

static void
eom_metadata_reader_png_class_init (EomMetadataReaderPngClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;

	object_class->dispose = eom_metadata_reader_png_dispose;
}

static gboolean
eom_metadata_reader_png_finished (EomMetadataReaderPng *emr)
{
	g_return_val_if_fail (EOM_IS_METADATA_READER_PNG (emr), TRUE);

	return (emr->priv->state == EMR_FINISHED);
}

static void
eom_metadata_reader_png_get_next_block (EomMetadataReaderPngPrivate* priv,
				    	guchar *chunk,
					int* i,
					const guchar *buf,
					int len,
					EomMetadataReaderPngState state)
{
	if (*i + priv->size < len) {
		/* read data in one block */
		memcpy ((guchar*) (chunk) + priv->bytes_read, &buf[*i], priv->size);
		priv->state = EMR_CHECK_CRC;
		*i = *i + priv->size - 1; /* the for-loop consumes the other byte */
		priv->size = 0;
	} else {
		int chunk_len = len - *i;
		memcpy ((guchar*) (chunk) + priv->bytes_read, &buf[*i], chunk_len);
		priv->bytes_read += chunk_len; /* bytes already read */
		priv->size = (*i + priv->size) - len; /* remaining data to read */
		*i = len - 1;
		priv->state = state;
	}
}

static void
eom_metadata_reader_png_consume (EomMetadataReaderPng *emr, const guchar *buf, guint len)
{
	EomMetadataReaderPngPrivate *priv;
 	int i;
	guint32 chunk_crc;
	static const gchar PNGMAGIC[8] = "\x89PNG\x0D\x0A\x1a\x0A";

	g_return_if_fail (EOM_IS_METADATA_READER_PNG (emr));

	priv = emr->priv;

	if (priv->state == EMR_FINISHED) return;

	for (i = 0; (i < len) && (priv->state != EMR_FINISHED); i++) {

		switch (priv->state) {
		case EMR_READ_MAGIC:
			/* Check PNG magic string */
			if (priv->sub_step < 8 &&
			    (gchar)buf[i] == PNGMAGIC[priv->sub_step]) {
			    	if (priv->sub_step == 7)
					priv->state = EMR_READ_SIZE_HIGH_HIGH_BYTE;
				priv->sub_step++;
			} else {
				priv->state = EMR_FINISHED;
			}
			break;
		case EMR_READ_SIZE_HIGH_HIGH_BYTE:
			/* Read the high byte of the size's high word */
			priv->size |= (buf[i] & 0xFF) << 24;
			priv->state = EMR_READ_SIZE_HIGH_LOW_BYTE;
			break;
		case EMR_READ_SIZE_HIGH_LOW_BYTE:
			/* Read the low byte of the size's high word */
			priv->size |= (buf[i] & 0xFF) << 16;
			priv->state = EMR_READ_SIZE_LOW_HIGH_BYTE;
			break;
		case EMR_READ_SIZE_LOW_HIGH_BYTE:
			/* Read the high byte of the size's low word */
			priv->size |= (buf [i] & 0xff) << 8;
			priv->state = EMR_READ_SIZE_LOW_LOW_BYTE;
			break;
		case EMR_READ_SIZE_LOW_LOW_BYTE:
			/* Read the high byte of the size's low word */
			priv->size |= (buf [i] & 0xff);
			/* The maximum chunk length is 2^31-1 */
			if (G_LIKELY (priv->size <= (guint32) 0x7fffffff)) {
				priv->state = EMR_READ_CHUNK_NAME;
				/* Make sure sub_step is 0 before next step */
				priv->sub_step = 0;
			} else {
				priv->state = EMR_FINISHED;
				eom_debug_message (DEBUG_IMAGE_DATA,
						   "chunk size larger than "
						   "2^31-1; stopping parser");
			}

			break;
		case EMR_READ_CHUNK_NAME:
			/* Read the 4-byte chunk name */
			if (priv->sub_step > 3)
				g_assert_not_reached ();

			priv->chunk_name[priv->sub_step] = buf[i];

			if (priv->sub_step++ != 3)
				break;

			if (G_UNLIKELY (!priv->hasIHDR)) {
				/* IHDR should be the first chunk in a PNG */
				if (priv->size == 13
				    && memcmp (priv->chunk_name, "IHDR", 4) == 0){
					priv->hasIHDR = TRUE;
				} else {
					/* Stop parsing if it is not */
					priv->state = EMR_FINISHED;
				}
			}

			/* Try to identify the chunk by its name.
			 * Already do some sanity checks where possible */
			if (memcmp (priv->chunk_name, "iTXt", 4) == 0 &&
			    priv->size > (22 + 54) && priv->xmp_chunk == NULL) {
				priv->state = EMR_READ_XMP_ITXT;
			} else if (memcmp (priv->chunk_name, "iCCP", 4) == 0 &&
				   priv->icc_chunk == NULL) {
				priv->state = EMR_READ_ICCP;
			} else if (memcmp (priv->chunk_name, "sRGB", 4) == 0 &&
				   priv->sRGB_chunk == NULL && priv->size == 1) {
				priv->state = EMR_READ_SRGB;
			} else if (memcmp (priv->chunk_name, "cHRM", 4) == 0 &&
				   priv->cHRM_chunk == NULL && priv->size == 32) {
				priv->state = EMR_READ_CHRM;
			} else if (memcmp (priv->chunk_name, "gAMA", 4) == 0 &&
				   priv->gAMA_chunk == NULL && priv->size == 4) {
				priv->state = EMR_READ_GAMA;
			} else if (memcmp (priv->chunk_name, "IEND", 4) == 0) {
				priv->state = EMR_FINISHED;
			} else {
				/* Skip chunk + 4-byte CRC32 value */
				priv->size += 4;
				priv->state = EMR_SKIP_BYTES;
			}
			priv->sub_step = 0;
			break;
		case EMR_SKIP_CRC:
			/* Skip the 4-byte CRC32 value following every chunk */
			priv->size = 4;
		case EMR_SKIP_BYTES:
		/* Skip chunk and start reading the size of the next one */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Skip bytes: %" G_GSIZE_FORMAT,
					   priv->size);

			if (i + priv->size < len) {
				i = i + priv->size - 1; /* the for-loop consumes the other byte */
				priv->size = 0;
				priv->state = EMR_READ_SIZE_HIGH_HIGH_BYTE;
			}
			else {
				priv->size = (i + priv->size) - len;
				i = len - 1;
			}
			break;
		case EMR_CHECK_CRC:
			/* Read the chunks CRC32 value from the file,... */
			if (priv->sub_step == 0)
				priv->target_crc = 0;

			priv->target_crc |= buf[i] << ((3 - priv->sub_step) * 8);

			if (priv->sub_step++ != 3)
				break;

			/* ...generate the chunks CRC32,... */
			chunk_crc = crc32 (crc32 (0L, Z_NULL, 0), priv->chunk_name, 4);
			chunk_crc = crc32 (chunk_crc, *priv->crc_chunk, *priv->crc_len);

			eom_debug_message (DEBUG_IMAGE_DATA, "Checking CRC: Chunk: 0x%X - Target: 0x%X", chunk_crc, priv->target_crc);

			/* ...and check if they match. If they don't throw
			 * the chunk away and stop parsing. */
			if (priv->target_crc == chunk_crc) {
				priv->state = EMR_READ_SIZE_HIGH_HIGH_BYTE;
			} else {
				g_free (*priv->crc_chunk);
				*priv->crc_chunk = NULL;
				*priv->crc_len = 0;
				/* Stop parsing for security reasons */
				priv->state = EMR_FINISHED;
			}
			priv->sub_step = 0;
			break;
		case EMR_READ_XMP_ITXT:
			/* Extract an iTXt chunk possibly containing
			 * an XMP packet */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Read XMP Chunk - size: %"
					   G_GSIZE_FORMAT, priv->size);

			if (priv->xmp_chunk == NULL) {
				priv->xmp_chunk = g_new0 (guchar, priv->size);
				priv->xmp_len = priv->size;
				priv->crc_len = &priv->xmp_len;
				priv->bytes_read = 0;
				priv->crc_chunk = &priv->xmp_chunk;
			}
			eom_metadata_reader_png_get_next_block (priv,
							    priv->xmp_chunk,
							    &i, buf, len,
							    EMR_READ_XMP_ITXT);

			if (priv->state == EMR_CHECK_CRC) {
				/* Check if it is actually an XMP chunk.
				 * Throw it away if not.
				 * The check has 4 extra \0's to check
				 * if the chunk is configured correctly. */
				if (memcmp (priv->xmp_chunk, "XML:com.adobe.xmp\0\0\0\0\0", 22) != 0) {
					priv->state = EMR_SKIP_CRC;
					g_free (priv->xmp_chunk);
					priv->xmp_chunk = NULL;
					priv->xmp_len = 0;
				}
			}
			break;
		case EMR_READ_ICCP:
			/* Extract an iCCP chunk containing a
			 * deflated ICC profile. */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Read ICC Chunk - size: %"
					   G_GSIZE_FORMAT, priv->size);

			if (priv->icc_chunk == NULL) {
				priv->icc_chunk = g_new0 (guchar, priv->size);
				priv->icc_len = priv->size;
				priv->crc_len = &priv->icc_len;
				priv->bytes_read = 0;
				priv->crc_chunk = &priv->icc_chunk;
			}

			eom_metadata_reader_png_get_next_block (priv,
							    priv->icc_chunk,
							    &i, buf, len,
							    EMR_READ_ICCP);
			break;
		case EMR_READ_SRGB:
			/* Extract the sRGB chunk. Marks the image data as
			 * being in sRGB colorspace. */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Read sRGB Chunk - value: %u", *(buf+i));

			if (priv->sRGB_chunk == NULL) {
				priv->sRGB_chunk = g_new0 (guchar, priv->size);
				priv->sRGB_len = priv->size;
				priv->crc_len = &priv->sRGB_len;
				priv->bytes_read = 0;
				priv->crc_chunk = &priv->sRGB_chunk;
			}

			eom_metadata_reader_png_get_next_block (priv,
							    priv->sRGB_chunk,
							    &i, buf, len,
							    EMR_READ_SRGB);
			break;
		case EMR_READ_CHRM:
			/* Extract the cHRM chunk. Contains the coordinates of
			 * the image's whitepoint and primary chromacities. */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Read cHRM Chunk - size: %"
					   G_GSIZE_FORMAT, priv->size);

			if (priv->cHRM_chunk == NULL) {
				priv->cHRM_chunk = g_new0 (guchar, priv->size);
				priv->cHRM_len = priv->size;
				priv->crc_len = &priv->cHRM_len;
				priv->bytes_read = 0;
				priv->crc_chunk = &priv->cHRM_chunk;
			}

			eom_metadata_reader_png_get_next_block (priv,
							    priv->cHRM_chunk,
							    &i, buf, len,
							    EMR_READ_ICCP);
			break;
		case EMR_READ_GAMA:
			/* Extract the gAMA chunk containing the
			 * image's gamma value */
			eom_debug_message (DEBUG_IMAGE_DATA,
					   "Read gAMA-Chunk - size: %"
					   G_GSIZE_FORMAT, priv->size);

			if (priv->gAMA_chunk == NULL) {
				priv->gAMA_chunk = g_new0 (guchar, priv->size);
				priv->gAMA_len = priv->size;
				priv->crc_len = &priv->gAMA_len;
				priv->bytes_read = 0;
				priv->crc_chunk = &priv->gAMA_chunk;
			}

			eom_metadata_reader_png_get_next_block (priv,
							    priv->gAMA_chunk,
							    &i, buf, len,
							    EMR_READ_ICCP);
			break;
		default:
			g_assert_not_reached ();
		}
	}
}

#ifdef HAVE_EXEMPI

/* skip the chunk ID */
#define EOM_XMP_OFFSET (22)

static gpointer
eom_metadata_reader_png_get_xmp_data (EomMetadataReaderPng *emr )
{
	EomMetadataReaderPngPrivate *priv;
	XmpPtr xmp = NULL;

	g_return_val_if_fail (EOM_IS_METADATA_READER_PNG (emr), NULL);

	priv = emr->priv;

	if (priv->xmp_chunk != NULL) {
		xmp = xmp_new (priv->xmp_chunk+EOM_XMP_OFFSET,
			       priv->xmp_len-EOM_XMP_OFFSET);
	}

	return (gpointer) xmp;
}
#endif

#if defined(HAVE_LCMS) && defined(GDK_WINDOWING_X11)

#define EXTRACT_DOUBLE_UINT_BLOCK_OFFSET(chunk,offset,divider) \
		(double)(GUINT32_FROM_BE(*((guint32*)((chunk)+((offset)*4))))/(double)(divider))

/* This is the amount of memory the inflate output buffer gets increased by
 * while decompressing the ICC profile */
#define EOM_ICC_INFLATE_BUFFER_STEP 1024

/* I haven't seen ICC profiles larger than 1MB yet.
 * A maximum output buffer of 5MB should be enough. */
#define EOM_ICC_INFLATE_BUFFER_LIMIT (1024*1024*5)

/* Apparently an sRGB profile saved in cHRM and gAMA chunks does not compute
 * a profile that exactly matches the built-in sRGB profile and thus could
 * cause a slight color deviation. Try catching this case to allow fallback
 * to the built-in profile instead.
 */
static gboolean
_chrm_matches_srgb(const cmsCIExyY       *whitepoint,
		   const cmsCIExyYTRIPLE *primaries,
		         gdouble          gammaValue)
{
	/* PNGs gAMA value for 2.2 is only accurate to the 4th decimal point */
#define DOUBLE_EQUAL_MAX_DIFF 1e-4
#define DOUBLE_EQUAL(a,b) (fabs (a - b) < DOUBLE_EQUAL_MAX_DIFF)

	return (DOUBLE_EQUAL(gammaValue, 2.2)
		&& DOUBLE_EQUAL(whitepoint->x, 0.3127)
		&& DOUBLE_EQUAL(whitepoint->y, 0.329)
		&& DOUBLE_EQUAL(primaries->Red.x, 0.64)
		&& DOUBLE_EQUAL(primaries->Red.y, 0.33)
		&& DOUBLE_EQUAL(primaries->Green.x, 0.3)
		&& DOUBLE_EQUAL(primaries->Green.y, 0.6)
		&& DOUBLE_EQUAL(primaries->Blue.x, 0.15)
		&& DOUBLE_EQUAL(primaries->Blue.y, 0.06));
}

static gpointer
eom_metadata_reader_png_get_icc_profile (EomMetadataReaderPng *emr)
{
	EomMetadataReaderPngPrivate *priv;
	cmsHPROFILE profile = NULL;

	g_return_val_if_fail (EOM_IS_METADATA_READER_PNG (emr), NULL);

	priv = emr->priv;

	if (priv->icc_chunk) {
		gpointer outbuf;
		gsize offset = 0;
		z_stream zstr;
		int z_ret;

		/* Use default allocation functions */
		zstr.zalloc = Z_NULL;
		zstr.zfree = Z_NULL;
		zstr.opaque = Z_NULL;

		/* Skip the name of the ICC profile */
		while (*((guchar*)priv->icc_chunk+offset) != '\0')
			offset++;
		/* Ensure the compression method (deflate) */
		if (*((guchar*)priv->icc_chunk+(++offset)) != '\0')
			return NULL;
		++offset; //offset now points to the start of the deflated data

		/* Prepare the zlib data structure for decompression */
		zstr.next_in = priv->icc_chunk + offset;
		zstr.avail_in = priv->icc_len - offset;
		if (inflateInit (&zstr) != Z_OK) {
			return NULL;
		}

		/* Prepare output buffer and make zlib aware of it */
		outbuf = g_malloc (EOM_ICC_INFLATE_BUFFER_STEP);
		zstr.next_out = outbuf;
		zstr.avail_out = EOM_ICC_INFLATE_BUFFER_STEP;

		do {
			if (zstr.avail_out == 0) {
				/* The output buffer was not large enough to
				 * hold all the decompressed data. Increase its
				 * size and continue decompression. */
				gsize new_size = zstr.total_out + EOM_ICC_INFLATE_BUFFER_STEP;

				if (G_UNLIKELY (new_size > EOM_ICC_INFLATE_BUFFER_LIMIT)) {
					/* Enforce a memory limit for the output
					 * buffer to avoid possible OOM cases */
					inflateEnd (&zstr);
					g_free (outbuf);
					eom_debug_message (DEBUG_IMAGE_DATA, "ICC profile is too large. Ignoring.");
					return NULL;
				}
				outbuf = g_realloc(outbuf, new_size);
				zstr.avail_out = EOM_ICC_INFLATE_BUFFER_STEP;
				zstr.next_out = outbuf + zstr.total_out;
			}
			z_ret = inflate (&zstr, Z_SYNC_FLUSH);
		} while (z_ret == Z_OK);

		if (G_UNLIKELY (z_ret != Z_STREAM_END)) {
			eom_debug_message (DEBUG_IMAGE_DATA, "Error while inflating ICC profile: %s (%d)", zstr.msg, z_ret);
			inflateEnd (&zstr);
			g_free (outbuf);
			return NULL;
		}

		if (GDK_IS_X11_DISPLAY (gdk_display_get_default ())) {
			profile = cmsOpenProfileFromMem(outbuf, zstr.total_out);
		}
		inflateEnd (&zstr);
		g_free (outbuf);

		eom_debug_message (DEBUG_LCMS, "PNG has %s ICC profile", profile ? "valid" : "invalid");
	}

	if (!profile && priv->sRGB_chunk) {
		eom_debug_message (DEBUG_LCMS, "PNG is sRGB");
		/* If the file has an sRGB chunk the image data is in the sRGB
		 * colorspace. lcms has a built-in sRGB profile. */

		if (GDK_IS_X11_DISPLAY (gdk_display_get_default ())) {
			profile = cmsCreate_sRGBProfile ();
		}
	}

	if (!profile && priv->cHRM_chunk && priv->gAMA_chunk) {
		cmsCIExyY whitepoint;
		cmsCIExyYTRIPLE primaries;
		cmsToneCurve *gamma[3];
		double gammaValue;

		/* This uglyness extracts the chromacity and whitepoint values
		 * from a PNG's cHRM chunk. These can be accurate up to the
		 * 5th decimal point.
		 * They are saved as integer values multiplied by 100000. */

		eom_debug_message (DEBUG_LCMS, "Trying to calculate color profile");

		whitepoint.x = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 0, 100000);
		whitepoint.y = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 1, 100000);

		primaries.Red.x = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 2, 100000);
		primaries.Red.y = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 3, 100000);
		primaries.Green.x = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 4, 100000);
		primaries.Green.y = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 5, 100000);
		primaries.Blue.x = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 6, 100000);
		primaries.Blue.y = EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->cHRM_chunk, 7, 100000);

		whitepoint.Y = primaries.Red.Y = primaries.Green.Y = primaries.Blue.Y = 1.0;

		gammaValue = (double) 1.0/EXTRACT_DOUBLE_UINT_BLOCK_OFFSET (priv->gAMA_chunk, 0, 100000);
		eom_debug_message (DEBUG_LCMS, "Gamma %.5lf", gammaValue);

		/* Catch SRGB in cHRM/gAMA chunks and use accurate built-in
		 * profile instead of computing one that "gets close". */
		if(_chrm_matches_srgb (&whitepoint, &primaries, gammaValue)) {
			eom_debug_message (DEBUG_LCMS, "gAMA and cHRM match sRGB");
			if (GDK_IS_X11_DISPLAY (gdk_display_get_default ())) {
				profile = cmsCreate_sRGBProfile ();
			}
		} else {
			gamma[0] = gamma[1] = gamma[2] =
				cmsBuildGamma (NULL, gammaValue);
			if (GDK_IS_X11_DISPLAY (gdk_display_get_default ())) {
				profile = cmsCreateRGBProfile (&whitepoint, &primaries,
						gamma);
				cmsFreeToneCurve(gamma[0]);
			}
		}
	}

	return profile;
}
#endif

static void
eom_metadata_reader_png_init_emr_iface (gpointer g_iface, gpointer iface_data)
{
	EomMetadataReaderInterface *iface;

	iface = (EomMetadataReaderInterface*) g_iface;

	iface->consume =
		(void (*) (EomMetadataReader *self, const guchar *buf, guint len))
			eom_metadata_reader_png_consume;
	iface->finished =
		(gboolean (*) (EomMetadataReader *self))
			eom_metadata_reader_png_finished;
#if defined(HAVE_LCMS) && defined(GDK_WINDOWING_X11)
	iface->get_icc_profile =
		(cmsHPROFILE (*) (EomMetadataReader *self))
			eom_metadata_reader_png_get_icc_profile;
#endif
#ifdef HAVE_EXEMPI
	iface->get_xmp_ptr =
		(gpointer (*) (EomMetadataReader *self))
			eom_metadata_reader_png_get_xmp_data;
#endif
}
