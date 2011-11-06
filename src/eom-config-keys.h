/* Eye Of Mate - MateConf Keys Macros
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __EOM_CONFIG_KEYS_H__
#define __EOM_CONFIG_KEYS_H__

#define EOM_CONF_DIR				"/apps/eom"

#define EOM_CONF_DESKTOP_WALLPAPER		"/desktop/mate/background/picture_filename"
#define EOM_CONF_DESKTOP_CAN_SAVE		"/desktop/mate/lockdown/disable_save_to_disk"
#define EOM_CONF_DESKTOP_CAN_PRINT      	"/desktop/mate/lockdown/disable_printing"
#define EOM_CONF_DESKTOP_CAN_SETUP_PAGE 	"/desktop/mate/lockdown/disable_print_setup"

#define EOM_CONF_VIEW_BACKGROUND_COLOR		"/apps/eom/view/background-color"
#define EOM_CONF_VIEW_INTERPOLATE		"/apps/eom/view/interpolate"
#define EOM_CONF_VIEW_EXTRAPOLATE		"/apps/eom/view/extrapolate"
#define EOM_CONF_VIEW_SCROLL_WHEEL_ZOOM		"/apps/eom/view/scroll_wheel_zoom"
#define EOM_CONF_VIEW_ZOOM_MULTIPLIER		"/apps/eom/view/zoom_multiplier"
#define EOM_CONF_VIEW_AUTOROTATE                "/apps/eom/view/autorotate"
#define EOM_CONF_VIEW_TRANSPARENCY		"/apps/eom/view/transparency"
#define EOM_CONF_VIEW_TRANS_COLOR		"/apps/eom/view/trans_color"
#define EOM_CONF_VIEW_USE_BG_COLOR		"/apps/eom/view/use-background-color"

#define EOM_CONF_FULLSCREEN_LOOP		"/apps/eom/full_screen/loop"
#define EOM_CONF_FULLSCREEN_UPSCALE		"/apps/eom/full_screen/upscale"
#define EOM_CONF_FULLSCREEN_SECONDS		"/apps/eom/full_screen/seconds"

#define EOM_CONF_UI_TOOLBAR			"/apps/eom/ui/toolbar"
#define EOM_CONF_UI_STATUSBAR			"/apps/eom/ui/statusbar"
#define EOM_CONF_UI_IMAGE_COLLECTION		"/apps/eom/ui/image_collection"
#define EOM_CONF_UI_IMAGE_COLLECTION_POSITION	"/apps/eom/ui/image_collection_position"
#define EOM_CONF_UI_IMAGE_COLLECTION_RESIZABLE	"/apps/eom/ui/image_collection_resizable"
#define EOM_CONF_UI_SIDEBAR			"/apps/eom/ui/sidebar"
#define EOM_CONF_UI_SCROLL_BUTTONS		"/apps/eom/ui/scroll_buttons"
#define EOM_CONF_UI_DISABLE_TRASH_CONFIRMATION	"/apps/eom/ui/disable_trash_confirmation"
#define EOM_CONF_UI_FILECHOOSER_XDG_FALLBACK	"/apps/eom/ui/filechooser_xdg_fallback"
#define EOM_CONF_UI_PROPSDIALOG_NETBOOK_MODE	"/apps/eom/ui/propsdialog_netbook_mode"

#define EOM_CONF_PLUGINS_ACTIVE_PLUGINS         "/apps/eom/plugins/active_plugins"

#endif /* __EOM_CONFIG_KEYS_H__ */
