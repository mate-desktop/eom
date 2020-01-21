/* gcc eom-thumbnailer.c `pkg-config --cflags --libs glib-2.0 gio-2.0 MagickWand` -lm -o eom-thumbnailer */
/*
 * Copyright (C) 2020 MATE developers
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors: Robert Buj <rbuj@fedoraproject.org>
 *
 */

#define _GNU_SOURCE

#include <string.h>
#include <glib.h>
#include <gio/gio.h>

#include <math.h>
#include <stdlib.h>
#include <wand/magick_wand.h>

static int output_size = 256;
static gboolean g_fatal_warnings = FALSE;
static char **filenames = NULL;

static char *
get_target_uri (GFile *file)
{
	GFileInfo *info;
	char *target;

	info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, G_FILE_QUERY_INFO_NONE, NULL, NULL);
	if (info == NULL)
		return NULL;
	target = g_strdup (g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI));
	g_object_unref (info);

	return target;
}

static char *
get_target_path (GFile *input)
{
	if (g_file_has_uri_scheme (input, "trash") != FALSE ||
	    g_file_has_uri_scheme (input, "recent") != FALSE) {
		GFile *file;
		char *input_uri;
		char *input_path;

		input_uri = get_target_uri (input);
		file = g_file_new_for_uri (input_uri);
		g_free (input_uri);
		input_path = g_file_get_path (file);
		g_object_unref (file);
		return input_path;
	}
	return g_file_get_path (input);
}

static const GOptionEntry entries[] = {
	{ "size", 's', 0, G_OPTION_ARG_INT, &output_size, "Size of the thumbnail in pixels", NULL },
	{"g-fatal-warnings", 0, 0, G_OPTION_ARG_NONE, &g_fatal_warnings, "Make all warnings fatal", NULL},
	{ G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames, NULL, "[INPUT FILE] [OUTPUT FILE]" },
	{ NULL }
};

int main (int argc, char **argv)
{
#define ThrowWandException(wand) \
{ \
  char \
    *description; \
 \
  ExceptionType \
    severity; \
 \
  description=MagickGetException(wand,&severity); \
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
  description=(char *) MagickRelinquishMemory(description); \
  exit(-1); \
}
	MagickBooleanType status;
	MagickWand *magick_wand;
	char *input_filename;
	GError *error = NULL;
	GOptionContext *context;
	GFile *input;
	const char *output;
	int width, height;

	/* Options parsing */
	context = g_option_context_new ("Thumbnail images");
	g_option_context_add_main_entries (context, entries, NULL);

	if (g_option_context_parse (context, &argc, &argv, &error) == FALSE) {
		g_warning ("Couldn't parse command-line options: %s", error->message);
		g_error_free (error);
		return 1;
	}

	/* Set fatal warnings if required */
	if (g_fatal_warnings) {
		GLogLevelFlags fatal_mask;

		fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
		fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
		g_log_set_always_fatal (fatal_mask);
	}

	if (filenames == NULL || g_strv_length (filenames) != 2) {
		g_print ("Expects an input and an output file\n");
		return 1;
	}

	input = g_file_new_for_commandline_arg (filenames[0]);
	input_filename = get_target_path (input);
	g_object_unref (input);

	if (input_filename == NULL) {
		g_warning ("Could not get file path for %s", filenames[0]);
		return 1;
	}

	output = filenames[1];

	if (output_size == 0) {
		g_warning ("Output size could not be 0");
		return 1;
	}

	/* Read an image */
	MagickWandGenesis ();
	magick_wand = NewMagickWand ();
	status = MagickReadImage (magick_wand, input_filename);
	if (status == MagickFalse)
		ThrowWandException (magick_wand);

	/* Get the image's width and height */
	width = MagickGetImageWidth (magick_wand);
	height = MagickGetImageHeight (magick_wand);

	/* Thumbnail */
	if ((height > output_size) || (width > output_size)) {
		double scale;
		scale = (double)output_size / MAX (width, height);
		MagickThumbnailImage (magick_wand,
                                      floor (width * scale + 0.5),
                                      floor (height * scale + 0.5));
	}

	/* Write the image then destroy it */
	status = MagickWriteImages (magick_wand, output, MagickTrue);
	if (status == MagickFalse)
		ThrowWandException (magick_wand);
	magick_wand = DestroyMagickWand (magick_wand);
	MagickWandTerminus ();

	return 0;
}
