/* Eye Of Mate - Application Facade
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on evince code (shell/ev-application.h) by:
 * 	- Martin Kretzschmar <martink@gnome.org>
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
#include "config.h"
#endif

#include "eom-image.h"
#include "eom-session.h"
#include "eom-window.h"
#include "eom-application.h"
#include "eom-util.h"

#ifdef HAVE_DBUS
#include "totem-scrsaver.h"
#endif

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#ifdef HAVE_DBUS
#include "eom-application-service.h"
#include <dbus/dbus-glib-bindings.h>

#define APPLICATION_SERVICE_NAME "org.mate.eom.ApplicationService"
#endif

static void eom_application_load_accelerators (void);
static void eom_application_save_accelerators (void);

#define EOM_APPLICATION_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EOM_TYPE_APPLICATION, EomApplicationPrivate))

G_DEFINE_TYPE (EomApplication, eom_application, G_TYPE_OBJECT);

#ifdef HAVE_DBUS

/**
 * eom_application_register_service:
 * @application: An #EomApplication.
 *
 * Registers #EomApplication<!-- -->'s DBus service, to allow
 * remote calls. If the DBus service is already registered,
 * or there is any other connection error, returns %FALSE.
 *
 * Returns: %TRUE if the service was registered succesfully. %FALSE
 * otherwise.
 **/
gboolean
eom_application_register_service (EomApplication *application)
{
	static DBusGConnection *connection = NULL;
	DBusGProxy *driver_proxy;
	GError *err = NULL;
	guint request_name_result;

	if (connection) {
		g_warning ("Service already registered.");
		return FALSE;
	}

	connection = dbus_g_bus_get (DBUS_BUS_STARTER, &err);

	if (connection == NULL) {
		g_warning ("Service registration failed.");
		g_error_free (err);

		return FALSE;
	}

	driver_proxy = dbus_g_proxy_new_for_name (connection,
						  DBUS_SERVICE_DBUS,
						  DBUS_PATH_DBUS,
						  DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy,
                                        	APPLICATION_SERVICE_NAME,
						DBUS_NAME_FLAG_DO_NOT_QUEUE,
						&request_name_result, &err)) {
		g_warning ("Service registration failed.");
		g_clear_error (&err);
	}

	g_object_unref (driver_proxy);

	if (request_name_result == DBUS_REQUEST_NAME_REPLY_EXISTS) {
		return FALSE;
	}

	dbus_g_object_type_install_info (EOM_TYPE_APPLICATION,
					 &dbus_glib_eom_application_object_info);

	dbus_g_connection_register_g_object (connection,
					     "/org/mate/eom/Eom",
                                             G_OBJECT (application));

        application->scr_saver = totem_scrsaver_new ();
        g_object_set (application->scr_saver,
		      "reason", _("Running in fullscreen mode"),
		      NULL);

	return TRUE;
}
#endif /* ENABLE_DBUS */

static void
eom_application_class_init (EomApplicationClass *eom_application_class)
{
}

static void
eom_application_init (EomApplication *eom_application)
{
	const gchar *dot_dir = eom_util_dot_dir ();

	eom_session_init (eom_application);

	eom_application->toolbars_model = egg_toolbars_model_new ();

	egg_toolbars_model_load_names (eom_application->toolbars_model,
				       EOM_DATA_DIR "/eom-toolbar.xml");

	if (G_LIKELY (dot_dir != NULL))
		eom_application->toolbars_file = g_build_filename
			(dot_dir, "eom_toolbar.xml", NULL);

	if (!dot_dir || !egg_toolbars_model_load_toolbars (eom_application->toolbars_model,
					       eom_application->toolbars_file)) {

		egg_toolbars_model_load_toolbars (eom_application->toolbars_model,
						  EOM_DATA_DIR "/eom-toolbar.xml");
	}

	egg_toolbars_model_set_flags (eom_application->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);

	eom_application_load_accelerators ();
}

/**
 * eom_application_get_instance:
 *
 * Returns a singleton instance of #EomApplication currently running.
 * If not running yet, it will create one.
 *
 * Returns: a running #EomApplication.
 **/
EomApplication *
eom_application_get_instance (void)
{
	static EomApplication *instance;

	if (!instance) {
		instance = EOM_APPLICATION (g_object_new (EOM_TYPE_APPLICATION, NULL));
	}

	return instance;
}

static EomWindow *
eom_application_get_empty_window (EomApplication *application)
{
	EomWindow *empty_window = NULL;
	GList *windows;
	GList *l;

	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	windows = eom_application_get_windows (application);

	for (l = windows; l != NULL; l = l->next) {
		EomWindow *window = EOM_WINDOW (l->data);

		if (eom_window_is_empty (window)) {
			empty_window = window;
			break;
		}
	}

	g_list_free (windows);

	return empty_window;
}

/**
 * eom_application_open_window:
 * @application: An #EomApplication.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens and presents an empty #EomWindow to the user. If there is
 * an empty window already open, this will be used. Otherwise, a
 * new one will be instantiated.
 *
 * Returns: %FALSE if @application is invalid, %TRUE otherwise
 **/
gboolean
eom_application_open_window (EomApplication  *application,
			     guint32         timestamp,
			     EomStartupFlags flags,
			     GError        **error)
{
	GtkWidget *new_window = NULL;

	new_window = GTK_WIDGET (eom_application_get_empty_window (application));

	if (new_window == NULL) {
		new_window = eom_window_new (flags);
	}

	g_return_val_if_fail (EOM_IS_APPLICATION (application), FALSE);

	gtk_window_present_with_time (GTK_WINDOW (new_window),
				      timestamp);

	return TRUE;
}

static EomWindow *
eom_application_get_file_window (EomApplication *application, GFile *file)
{
	EomWindow *file_window = NULL;
	GList *windows;
	GList *l;

	g_return_val_if_fail (file != NULL, NULL);
	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	windows = gtk_window_list_toplevels ();

	for (l = windows; l != NULL; l = l->next) {
		if (EOM_IS_WINDOW (l->data)) {
			EomWindow *window = EOM_WINDOW (l->data);

			if (!eom_window_is_empty (window)) {
				EomImage *image = eom_window_get_image (window);
				GFile *window_file;

				window_file = eom_image_get_file (image);
				if (g_file_equal (window_file, file)) {
					file_window = window;
					break;
				}
			}
		}
	}

	g_list_free (windows);

	return file_window;
}

static void
eom_application_show_window (EomWindow *window, gpointer user_data)
{
	gtk_window_present_with_time (GTK_WINDOW (window),
				      GPOINTER_TO_UINT (user_data));
}

/**
 * eom_application_open_file_list:
 * @application: An #EomApplication.
 * @file_list: A list of #GFile<!-- -->s.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of files in a #EomWindow. If an #EomWindow displaying the first
 * image in the list is already open, this will be used. Otherwise, an empty
 * #EomWindow is used, either already existing or newly created.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_file_list (EomApplication  *application,
				GSList          *file_list,
				guint           timestamp,
				EomStartupFlags flags,
				GError         **error)
{
	EomWindow *new_window = NULL;

	if (file_list != NULL)
		new_window = eom_application_get_file_window (application,
							      (GFile *) file_list->data);

	if (new_window != NULL) {
		gtk_window_present_with_time (GTK_WINDOW (new_window),
					      timestamp);
		return TRUE;
	}

	new_window = eom_application_get_empty_window (application);

	if (new_window == NULL) {
		new_window = EOM_WINDOW (eom_window_new (flags));
	}

	g_signal_connect (new_window,
			  "prepared",
			  G_CALLBACK (eom_application_show_window),
			  GUINT_TO_POINTER (timestamp));

	eom_window_open_file_list (new_window, file_list);

	return TRUE;
}

/**
 * eom_application_open_uri_list:
 * @application: An #EomApplication.
 * @uri_list: A list of URIs.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of images, from a list of URIs. See
 * eom_application_open_file_list() for details.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_uri_list (EomApplication  *application,
 			       GSList          *uri_list,
 			       guint           timestamp,
 			       EomStartupFlags flags,
 			       GError         **error)
{
 	GSList *file_list = NULL;

 	g_return_val_if_fail (EOM_IS_APPLICATION (application), FALSE);

 	file_list = eom_util_string_list_to_file_list (uri_list);

 	return eom_application_open_file_list (application,
					       file_list,
					       timestamp,
					       flags,
					       error);
}

#ifdef HAVE_DBUS
/**
 * eom_application_open_uris:
 * @application: an #EomApplication
 * @uris:  A #GList of URI strings.
 * @timestamp: The timestamp of the user interaction which triggered this call
 * (see gtk_window_present_with_time()).
 * @flags: A set of #EomStartupFlags influencing a new windows' state.
 * @error: Return location for a #GError, or NULL to ignore errors.
 *
 * Opens a list of images, from a list of URI strings. See
 * eom_application_open_file_list() for details.
 *
 * Returns: Currently always %TRUE.
 **/
gboolean
eom_application_open_uris (EomApplication  *application,
 			   gchar          **uris,
 			   guint           timestamp,
 			   EomStartupFlags flags,
 			   GError        **error)
{
 	GSList *file_list = NULL;

 	file_list = eom_util_strings_to_file_list (uris);

 	return eom_application_open_file_list (application, file_list, timestamp,
						    flags, error);
}
#endif

/**
 * eom_application_shutdown:
 * @application: An #EomApplication.
 *
 * Takes care of shutting down the Eye of MATE, and quits.
 **/
void
eom_application_shutdown (EomApplication *application)
{
	g_return_if_fail (EOM_IS_APPLICATION (application));

	if (application->toolbars_model) {
		g_object_unref (application->toolbars_model);
		application->toolbars_model = NULL;

		g_free (application->toolbars_file);
		application->toolbars_file = NULL;
	}

	eom_application_save_accelerators ();

	g_object_unref (application);

	gtk_main_quit ();
}

/**
 * eom_application_get_windows:
 * @application: An #EomApplication.
 *
 * Gets the list of existing #EomApplication<!-- -->s. The windows
 * in this list are not individually referenced, you need to keep
 * your own references if you want to perform actions that may destroy
 * them.
 *
 * Returns: A new list of #EomWindow<!-- -->s.
 **/
GList *
eom_application_get_windows (EomApplication *application)
{
	GList *l, *toplevels;
	GList *windows = NULL;

	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	toplevels = gtk_window_list_toplevels ();

	for (l = toplevels; l != NULL; l = l->next) {
		if (EOM_IS_WINDOW (l->data)) {
			windows = g_list_append (windows, l->data);
		}
	}

	g_list_free (toplevels);

	return windows;
}

/**
 * eom_application_get_toolbars_model:
 * @application: An #EomApplication.
 *
 * Retrieves the #EggToolbarsModel for the toolbar in #EomApplication.
 *
 * Returns: An #EggToolbarsModel.
 **/
EggToolbarsModel *
eom_application_get_toolbars_model (EomApplication *application)
{
	g_return_val_if_fail (EOM_IS_APPLICATION (application), NULL);

	return application->toolbars_model;
}

/**
 * eom_application_save_toolbars_model:
 * @application: An #EomApplication.
 *
 * Causes the saving of the model of the toolbar in #EomApplication to a file.
 **/
void
eom_application_save_toolbars_model (EomApplication *application)
{
	if (G_LIKELY(application->toolbars_file != NULL))
        	egg_toolbars_model_save_toolbars (application->toolbars_model,
				 	          application->toolbars_file,
						  "1.0");
}

/**
 * eom_application_reset_toolbars_model:
 * @app: an #EomApplication
 *
 * Restores the toolbars model to the defaults.
 **/
void
eom_application_reset_toolbars_model (EomApplication *app)
{
	g_return_if_fail (EOM_IS_APPLICATION (app));

	g_object_unref (app->toolbars_model);

	app->toolbars_model = egg_toolbars_model_new ();

	egg_toolbars_model_load_names (app->toolbars_model,
				       EOM_DATA_DIR "/eom-toolbar.xml");
	egg_toolbars_model_load_toolbars (app->toolbars_model,
					  EOM_DATA_DIR "/eom-toolbar.xml");
	egg_toolbars_model_set_flags (app->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);
}

#ifdef HAVE_DBUS
/**
 * eom_application_screensaver_enable:
 * @application: an #EomApplication.
 *
 * Enables the screensaver. Usually necessary after a call to
 * eom_application_screensaver_disable().
 **/
void
eom_application_screensaver_enable (EomApplication *application)
{
        if (application->scr_saver)
                totem_scrsaver_enable (application->scr_saver);
}

/**
 * eom_application_screensaver_disable:
 * @application: an #EomApplication.
 *
 * Disables the screensaver. Useful when the application is in fullscreen or
 * similar mode.
 **/
void
eom_application_screensaver_disable (EomApplication *application)
{
        if (application->scr_saver)
                totem_scrsaver_disable (application->scr_saver);
}
#endif

static void
eom_application_load_accelerators (void)
{
	gchar *accelfile = g_build_filename (g_get_home_dir (),
					     ".mate2",
					     "accels",
					     "eom", NULL);

	/* gtk_accel_map_load does nothing if the file does not exist */
	gtk_accel_map_load (accelfile);
	g_free (accelfile);
}

static void
eom_application_save_accelerators (void)
{
	gchar *accelfile = g_build_filename (g_get_home_dir (),
					     ".mate2",
					     "accels",
					     "eom", NULL);

	gtk_accel_map_save (accelfile);
	g_free (accelfile);
}
