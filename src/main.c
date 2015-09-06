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
#ifdef HAVE_DBUS
#include <dbus/dbus-glib-bindings.h>
#include <gdk/gdkx.h>
#endif
#ifdef HAVE_INSTROSPECTION
#include <girepository.h>
#endif

#include "eom-session.h"
#include "eom-debug.h"
#include "eom-thumbnail.h"
#include "eom-job-queue.h"
#include "eom-application.h"
#include "eom-plugin-engine.h"
#include "eom-util.h"

#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>

#if HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

#if GTK_CHECK_VERSION (3, 0, 0)
#define EOM_CSS_FILE_PATH EOM_DATA_DIR G_DIR_SEPARATOR_S "eom.css"
#endif

static EomStartupFlags flags;

static gboolean fullscreen = FALSE;
static gboolean slide_show = FALSE;
static gboolean disable_collection = FALSE;
#if HAVE_DBUS
static gboolean force_new_instance = FALSE;
#endif
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
#if HAVE_DBUS
	{ "new-instance", 'n', 0, G_OPTION_ARG_NONE, &force_new_instance, N_("Start a new instance instead of reusing an existing one"), NULL },
#endif
	{ "version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
	  _print_version_and_exit, N_("Show the application's version"), NULL},
	{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &startup_files, NULL, N_("[FILE…]") },
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

static void
load_files (void)
{
	GSList *files = NULL;

	files = eom_util_string_array_to_list ((const gchar **) startup_files, TRUE);

	eom_application_open_uri_list (EOM_APP,
				       files,
				       GDK_CURRENT_TIME,
				       flags,
				       NULL);

	g_slist_foreach (files, (GFunc) g_free, NULL);
	g_slist_free (files);
}

#ifdef HAVE_DBUS
static gboolean
load_files_remote (void)
{
	GError *error = NULL;
	DBusGConnection *connection;
	DBusGProxy *remote_object;
	gboolean result = TRUE;
	GdkDisplay *display;
	guint32 timestamp;
	gchar **files;

	display = gdk_display_get_default ();

	timestamp = gdk_x11_display_get_user_time (display);
	connection = dbus_g_bus_get (DBUS_BUS_STARTER, &error);

	if (connection == NULL) {
		g_warning ("%s", error->message);
		g_error_free (error);

 		return FALSE;
 	}

 	files = eom_util_string_array_make_absolute (startup_files);

 	remote_object = dbus_g_proxy_new_for_name (connection,
 						   "org.mate.eom.ApplicationService",
						   "/org/mate/eom/Eom",
						   "org.mate.eom.Application");

 	if (!files) {
 		if (!dbus_g_proxy_call (remote_object, "OpenWindow", &error,
 					G_TYPE_UINT, timestamp,
 					G_TYPE_UCHAR, flags,
 					G_TYPE_INVALID,
 					G_TYPE_INVALID)) {
 			g_warning ("%s", error->message);
 			g_clear_error (&error);

 			result = FALSE;
 		}
 	} else {
 		if (!dbus_g_proxy_call (remote_object, "OpenUris", &error,
 					G_TYPE_STRV, files,
 					G_TYPE_UINT, timestamp,
 					G_TYPE_UCHAR, flags,
 					G_TYPE_INVALID,
 					G_TYPE_INVALID)) {
 			g_warning ("%s", error->message);
 			g_clear_error (&error);

			result = FALSE;
 		}

 		g_strfreev (files);
 	}

 	g_object_unref (remote_object);
 	dbus_g_connection_unref (connection);

 	gdk_notify_startup_complete ();

 	return result;
}
#endif /* HAVE_DBUS */

int
main (int argc, char **argv)
{
	GError *error = NULL;
	GOptionContext *ctx;
#if GTK_CHECK_VERSION (3, 0, 0)
	GtkCssProvider *provider;
#endif

	bindtextdomain (GETTEXT_PACKAGE, EOM_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

#if !GTK_CHECK_VERSION (3, 0, 0)
	gtk_rc_parse (EOM_DATA_DIR G_DIR_SEPARATOR_S "gtkrc");
#endif

	ctx = g_option_context_new (NULL);
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

#ifdef HAVE_DBUS
	if (!force_new_instance &&
	    !eom_application_register_service (EOM_APP)) {
		if (load_files_remote ()) {
			return 0;
		}
	}
#endif /* HAVE_DBUS */

#ifdef HAVE_EXEMPI
 	xmp_init();
#endif
	eom_debug_init ();
	eom_job_queue_init ();
	eom_thumbnail_init ();

#if GTK_CHECK_VERSION (3, 0, 0)
	/* Load special style properties for EomThumbView's scrollbar */
	provider = gtk_css_provider_new ();
	if (G_UNLIKELY (!gtk_css_provider_load_from_path(provider,
							 EOM_CSS_FILE_PATH,
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
#endif

	eom_plugin_engine_init ();

	/* Add application specific icons to search path */
	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
                                           EOM_DATA_DIR G_DIR_SEPARATOR_S "icons");

	gtk_window_set_default_icon_name ("eom");
	g_set_application_name (_("Eye of MATE Image Viewer"));

	load_files ();

	gdk_threads_enter ();

	gtk_main ();

	gdk_threads_leave ();

  	if (startup_files)
		g_strfreev (startup_files);

	eom_plugin_engine_shutdown ();

#ifdef HAVE_EXEMPI
	xmp_terminate();
#endif
	return 0;
}
