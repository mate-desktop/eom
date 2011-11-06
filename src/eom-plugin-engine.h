/* Eye Of Mate - EOM Plugin Engine
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-plugins-engine.h) by:
 * 	- Paolo Maggi <paolo@gnome.org>
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

#ifndef __EOM_PLUGIN_ENGINE_H__
#define __EOM_PLUGIN_ENGINE_H__

#include "eom-window.h"

#include <glib.h>

typedef struct _EomPluginInfo EomPluginInfo;

G_GNUC_INTERNAL
gboolean	 eom_plugin_engine_init 		(void);

G_GNUC_INTERNAL
void		 eom_plugin_engine_shutdown 		(void);

G_GNUC_INTERNAL
void		 eom_plugin_engine_garbage_collect	(void);

G_GNUC_INTERNAL
const GList	*eom_plugin_engine_get_plugins_list 	(void);

G_GNUC_INTERNAL
gboolean 	 eom_plugin_engine_activate_plugin 	(EomPluginInfo *info);

G_GNUC_INTERNAL
gboolean 	 eom_plugin_engine_deactivate_plugin	(EomPluginInfo *info);

G_GNUC_INTERNAL
gboolean 	 eom_plugin_engine_plugin_is_active 	(EomPluginInfo *info);

G_GNUC_INTERNAL
gboolean 	 eom_plugin_engine_plugin_is_available	(EomPluginInfo *info);

G_GNUC_INTERNAL
gboolean	 eom_plugin_engine_plugin_is_configurable
			       				(EomPluginInfo *info);

G_GNUC_INTERNAL
void	 	 eom_plugin_engine_configure_plugin	(EomPluginInfo *info,
			       			 	 GtkWindow     *parent);

G_GNUC_INTERNAL
void		 eom_plugin_engine_update_plugins_ui	(EomWindow     *window,
			       			 	 gboolean       new_window);

G_GNUC_INTERNAL
const gchar	*eom_plugin_engine_get_plugin_name	(EomPluginInfo *info);

G_GNUC_INTERNAL
const gchar	*eom_plugin_engine_get_plugin_description
			       				(EomPluginInfo *info);

G_GNUC_INTERNAL
const gchar	*eom_plugin_engine_get_plugin_icon_name (EomPluginInfo *info);

G_GNUC_INTERNAL
const gchar    **eom_plugin_engine_get_plugin_authors   (EomPluginInfo *info);

G_GNUC_INTERNAL
const gchar	*eom_plugin_engine_get_plugin_website   (EomPluginInfo *info);

G_GNUC_INTERNAL
const gchar	*eom_plugin_engine_get_plugin_copyright (EomPluginInfo *info);

#endif  /* __EOM_PLUGIN_ENGINE_H__ */
