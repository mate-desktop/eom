/* Eye Of Mate - EOM Plugin
 *
 * Copyright (C) 2007 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on gedit code (gedit/gedit-module.c) by:
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "eom-plugin.h"

G_DEFINE_TYPE (EomPlugin, eom_plugin, G_TYPE_OBJECT)

static void
dummy (EomPlugin *plugin, EomWindow *window)
{
}

static GtkWidget *
create_configure_dialog	(EomPlugin *plugin)
{
	return NULL;
}

static gboolean
is_configurable (EomPlugin *plugin)
{
	return (EOM_PLUGIN_GET_CLASS (plugin)->create_configure_dialog !=
		create_configure_dialog);
}

static void
eom_plugin_class_init (EomPluginClass *klass)
{
	klass->activate = dummy;
	klass->deactivate = dummy;
	klass->update_ui = dummy;

	klass->create_configure_dialog = create_configure_dialog;
	klass->is_configurable = is_configurable;
}

static void
eom_plugin_init (EomPlugin *plugin)
{
}

void
eom_plugin_activate (EomPlugin *plugin, EomWindow *window)
{
	g_return_if_fail (EOM_IS_PLUGIN (plugin));
	g_return_if_fail (EOM_IS_WINDOW (window));

	EOM_PLUGIN_GET_CLASS (plugin)->activate (plugin, window);
}

void
eom_plugin_deactivate (EomPlugin *plugin, EomWindow *window)
{
	g_return_if_fail (EOM_IS_PLUGIN (plugin));
	g_return_if_fail (EOM_IS_WINDOW (window));

	EOM_PLUGIN_GET_CLASS (plugin)->deactivate (plugin, window);
}

void
eom_plugin_update_ui (EomPlugin *plugin, EomWindow *window)
{
	g_return_if_fail (EOM_IS_PLUGIN (plugin));
	g_return_if_fail (EOM_IS_WINDOW (window));

	EOM_PLUGIN_GET_CLASS (plugin)->update_ui (plugin, window);
}

gboolean
eom_plugin_is_configurable (EomPlugin *plugin)
{
	g_return_val_if_fail (EOM_IS_PLUGIN (plugin), FALSE);

	return EOM_PLUGIN_GET_CLASS (plugin)->is_configurable (plugin);
}

GtkWidget *
eom_plugin_create_configure_dialog (EomPlugin *plugin)
{
	g_return_val_if_fail (EOM_IS_PLUGIN (plugin), NULL);

	return EOM_PLUGIN_GET_CLASS (plugin)->create_configure_dialog (plugin);
}
