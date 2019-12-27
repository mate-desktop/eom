/* Eye Of Mate - Main
 *
 * Copyright (C) 2000-2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on code by:
 * 	- Federico Mena-Quintero <federico@gnu.org>
 *	- Jens Finke <jens@gnome.org>
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
#include "config.h"
#endif
#ifdef HAVE_INTROSPECTION
#include <girepository.h>
#endif

#include "eom-session.h"
#include "eom-debug.h"
#include "eom-thumbnail.h"
#include "eom-job-queue.h"
#include "eom-application.h"
#include "eom-application-internal.h"
#include "eom-util.h"

#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>

#if HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

#define EOM_CSS_FILE_PATH EOM_DATA_DIR G_DIR_SEPARATOR_S "eom.css"

static EomStartupFlags flags;

static gboolean fullscreen = FALSE;
static gboolean slide_show = FALSE;
static gboolean disable_collection = FALSE;
static gboolean force_new_instance = FALSE;
static gchar **startup_files = NULL;

static gboolean
_print_version_and_exit (const gchar *option_name,
			 const gchar *value,
			 gpointer data,
			 GError **error)
{
	g_print("%s %s\n", _("Eye of MATE Image Viewer"), VERSION);
	exit (EXIT_SUCCESS);
	return TRUE;
}

static const GOptionEntry goption_options[] =
{
	{ "fullscreen", 'f', 0, G_OPTION_ARG_NONE, &fullscreen, N_("Open in fullscreen mode"), NULL  },
	{ "disable-image-collection", 'c', 0, G_OPTION_ARG_NONE, &disable_collection, N_("Disable image collection"), NULL  },
	{ "slide-show", 's', 0, G_OPTION_ARG_NONE, &slide_show, N_("Open in slideshow mode"), NULL  },
	{ "new-instance", 'n', 0, G_OPTION_ARG_NONE, &force_new_instance, N_("Start a new instance instead of reusing an existing one"), NULL },
	{ "version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
	  _print_version_and_exit, N_("Show the application's version"), NULL},
	{ NULL }
};

static void
set_startup_flags (void)
{
  if (fullscreen)
    flags |= EOM_STARTUP_FULLSCREEN;

  if (disable_collection)
    flags |= EOM_STARTUP_DISABLE_COLLECTION;

  if (slide_show)
    flags |= EOM_STARTUP_SLIDE_SHOW;
}

int
main (int argc, char **argv)
{
	GError *error = NULL;
	GOptionContext *ctx;
	GFile *css_file;
	GtkCssProvider *provider;

	bindtextdomain (GETTEXT_PACKAGE, EOM_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	gdk_set_allowed_backends ("wayland,x11");

	ctx = g_option_context_new (_("[FILE…]"));
	g_option_context_add_main_entries (ctx, goption_options, PACKAGE);
	/* Option groups are free'd together with the context
	 * Using gtk_get_option_group here initializes gtk during parsing */
	g_option_context_add_group (ctx, gtk_get_option_group (TRUE));
#ifdef HAVE_INTROSPECTION
	g_option_context_add_group (ctx, g_irepository_get_option_group ());
#endif

	if (!g_option_context_parse (ctx, &argc, &argv, &error)) {
		gchar *help_msg;

		/* I18N: The '%s' is replaced with eom's command name. */
		help_msg = g_strdup_printf (_("Run '%s --help' to see a full "
					      "list of available command line "
					      "options."), argv[0]);
                g_printerr ("%s\n%s\n", error->message, help_msg);
                g_error_free (error);
		g_free (help_msg);
                g_option_context_free (ctx);

                return 1;
        }
	g_option_context_free (ctx);


	set_startup_flags ();

#ifdef HAVE_EXEMPI
 	xmp_init();
#endif
	eom_debug_init ();
	eom_job_queue_init ();
	eom_thumbnail_init ();

	/* Load special style properties for EomThumbView's scrollbar */
	css_file = g_file_new_for_uri ("resource:///org/mate/eom/ui/eom.css");
	provider = gtk_css_provider_new ();
	if (G_UNLIKELY (!gtk_css_provider_load_from_file(provider,
	                                                 css_file,
	                                                 &error)))
	{
		g_critical ("Could not load CSS data: %s", error->message);
		g_clear_error (&error);
	} else {
		gtk_style_context_add_provider_for_screen (
				gdk_screen_get_default(),
				GTK_STYLE_PROVIDER (provider),
				GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}
	g_object_unref (provider);
	g_object_unref (css_file);

	/* Add application specific icons to search path */
	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
                                           EOM_DATA_DIR G_DIR_SEPARATOR_S "icons");

	gtk_window_set_default_icon_name ("eom");
	g_set_application_name (_("Eye of MATE Image Viewer"));

	EOM_APP->priv->flags = flags;
	if (force_new_instance) {
		GApplicationFlags app_flags = g_application_get_flags (G_APPLICATION (EOM_APP));
		app_flags |= G_APPLICATION_NON_UNIQUE;
		g_application_set_flags (G_APPLICATION (EOM_APP), app_flags);
	}

	g_application_run (G_APPLICATION (EOM_APP), argc, argv);
	g_object_unref (EOM_APP);

  	if (startup_files)
		g_strfreev (startup_files);

#ifdef HAVE_EXEMPI
	xmp_terminate();
#endif
	return 0;
}
