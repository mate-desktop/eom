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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <glib.h>
#include <gio/gio.h>

#include <math.h>
#include <stdlib.h>

#ifdef HAVE_IMAGEMAGICK7
#include <MagickWand/MagickWand.h>
#endif
#ifdef HAVE_IMAGEMAGICK6
#include <wand/magick_wand.h>
#endif

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
	{"g-fatal-warnings", '\0', 0, G_OPTION_ARG_NONE, &g_fatal_warnings, "Make all warnings fatal", NULL},
	{ G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames, NULL, "[INPUT FILE] [OUTPUT FILE]" },
	{ 0, '\0', 0, G_OPTION_ARG_NONE, 0, 0, 0 }
};

int main (int argc, char **argv)
{
	MagickBooleanType status;
	MagickWand *magick_wand;
	char *input_filename;
	GError *error = NULL;
	GOptionContext *context;
	GFile *input;
	const char *output;
	size_t width, height;

	/* Options parsing */
	context = g_option_context_new ("- thumbnail images");
	g_option_context_add_main_entries (context, entries, NULL);

	(void) g_option_context_parse (context, &argc, &argv, &error);
	g_option_context_free (context);
	if (error) {
		g_warning ("Couldn't parse command-line options: %s", error->message);
		g_error_free (error);
		goto arguments_error;
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
		goto arguments_error;
	}

	if (output_size < 1) {
		g_warning ("Size cannot be smaller than 1 pixel");
		goto arguments_error;
	}

	input = g_file_new_for_commandline_arg (filenames[0]);
	input_filename = get_target_path (input);
	g_object_unref (input);
	if (input_filename == NULL) {
		g_warning ("Could not get file path for %s", filenames[0]);
		goto arguments_error;
	}

	output = filenames[1];

	/* Read an image */
	MagickWandGenesis ();
	magick_wand = NewMagickWand ();
	status = MagickReadImage (magick_wand, input_filename);
	g_free (input_filename);
	if (status == MagickFalse) {
		g_warning ("Could not load input file %s", filenames[0]);
		goto imagemagick_error;
	}

	/* Get the image's width and height */
	width = MagickGetImageWidth (magick_wand);
	height = MagickGetImageHeight (magick_wand);

	/* Thumbnail */
	if ((height > output_size) || (width > output_size)) {
		double scale;
		scale = (double) output_size / MAX (width, height);
		MagickThumbnailImage (magick_wand,
                                      (size_t) floor (width * scale + 0.5),
                                      (size_t) floor (height * scale + 0.5));
	}

	/* Write the image then destroy it */
	status = MagickWriteImages (magick_wand, output, MagickTrue);
	if (status == MagickFalse) {
		g_warning ("Could not save output file %s", output);
		goto imagemagick_error;
	}
	g_strfreev (filenames);
	if (magick_wand)
		DestroyMagickWand (magick_wand);
	MagickWandTerminus ();

	return 0;

arguments_error:
	if (filenames)
		g_strfreev (filenames);
	return 1;

imagemagick_error:
	g_strfreev (filenames);
	if (magick_wand)
		DestroyMagickWand (magick_wand);
	return 1;
}
