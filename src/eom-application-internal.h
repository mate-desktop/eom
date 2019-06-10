/* Eye Of Gnome - Application Facade (internal)
 *
 * Copyright (C) 2006-2012 The Free Software Foundation
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_APPLICATION_INTERNAL_H__
#define __EOM_APPLICATION_INTERNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <libpeas/peas-extension-set.h>

#include "eom-application.h"
#include "eom-plugin-engine.h"
#include "egg-toolbars-model.h"
#include "eom-window.h"

G_BEGIN_DECLS

struct _EomApplicationPrivate {
	EggToolbarsModel *toolbars_model;
	gchar            *toolbars_file;
	EomPluginEngine  *plugin_engine;

	EomStartupFlags   flags;

	PeasExtensionSet *extensions;
};


EggToolbarsModel *eom_application_get_toolbars_model  (EomApplication *application);

void              eom_application_save_toolbars_model (EomApplication *application);

void              eom_application_reset_toolbars_model (EomApplication *app);

G_END_DECLS

#endif /* __EOM_APPLICATION_INTERNAL_H__ */

