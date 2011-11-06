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

#ifndef __EOM_APPLICATION_H__
#define __EOM_APPLICATION_H__

#include "eom-window.h"
#include "egg-toolbars-model.h"

#ifdef HAVE_DBUS
#include "totem-scrsaver.h"
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _EomApplication EomApplication;
typedef struct _EomApplicationClass EomApplicationClass;
typedef struct _EomApplicationPrivate EomApplicationPrivate;

#define EOM_TYPE_APPLICATION            (eom_application_get_type ())
#define EOM_APPLICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_APPLICATION, EomApplication))
#define EOM_APPLICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_APPLICATION, EomApplicationClass))
#define EOM_IS_APPLICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_APPLICATION))
#define EOM_IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  EOM_TYPE_APPLICATION))
#define EOM_APPLICATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_APPLICATION, EomApplicationClass))

#define EOM_APP				(eom_application_get_instance ())

struct _EomApplication {
	GObject base_instance;

	EggToolbarsModel *toolbars_model;
	gchar            *toolbars_file;
#ifdef HAVE_DBUS
	TotemScrsaver    *scr_saver;
#endif
};

struct _EomApplicationClass {
	GObjectClass parent_class;
};

GType	          eom_application_get_type	      (void) G_GNUC_CONST;

EomApplication   *eom_application_get_instance        (void);

#ifdef HAVE_DBUS
gboolean          eom_application_register_service    (EomApplication *application);
#endif

void	          eom_application_shutdown	      (EomApplication   *application);

gboolean          eom_application_open_window         (EomApplication   *application,
						       guint             timestamp,
						       EomStartupFlags   flags,
						       GError          **error);

gboolean          eom_application_open_uri_list      (EomApplication   *application,
						      GSList           *uri_list,
						      guint             timestamp,
						      EomStartupFlags   flags,
						      GError          **error);

gboolean          eom_application_open_file_list     (EomApplication  *application,
						      GSList          *file_list,
						      guint           timestamp,
						      EomStartupFlags flags,
						      GError         **error);

#ifdef HAVE_DBUS
gboolean          eom_application_open_uris           (EomApplication *application,
						       gchar         **uris,
						       guint           timestamp,
						       EomStartupFlags flags,
						       GError        **error);
#endif

GList		 *eom_application_get_windows	      (EomApplication   *application);

EggToolbarsModel *eom_application_get_toolbars_model  (EomApplication *application);

void              eom_application_save_toolbars_model (EomApplication *application);

void		  eom_application_reset_toolbars_model (EomApplication *app);

#ifdef HAVE_DBUS
void              eom_application_screensaver_enable  (EomApplication *application);

void              eom_application_screensaver_disable (EomApplication *application);
#endif

G_END_DECLS

#endif /* __EOM_APPLICATION_H__ */
