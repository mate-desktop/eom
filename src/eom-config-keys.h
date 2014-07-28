/* Eye Of Mate - GSettings Keys and Schemas definitions
 *
 * Copyright (C) 2000-2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *         Stefano Karapetsas <stefano@karapetsas.com>
 *
 * Based on code by:
 *  - Federico Mena-Quintero <federico@gnu.org>
 *  - Jens Finke <jens@gnome.org>
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

#ifndef __EOM_CONFIG_KEYS_H__
#define __EOM_CONFIG_KEYS_H__

#define EOM_CONF_DOMAIN				"org.mate.eom"
#define EOM_CONF_FULLSCREEN			EOM_CONF_DOMAIN".full-screen"
#define EOM_CONF_PLUGINS			EOM_CONF_DOMAIN".plugins"
#define EOM_CONF_UI				EOM_CONF_DOMAIN".ui"
#define EOM_CONF_VIEW				EOM_CONF_DOMAIN".view"

#define EOM_CONF_BACKGROUND_SCHEMA              "org.mate.background"
#define EOM_CONF_BACKGROUND_FILE                "picture-filename"

#define EOM_CONF_LOCKDOWN_SCHEMA                "org.mate.lockdown"
#define EOM_CONF_LOCKDOWN_CAN_SAVE              "disable-save-to-disk"
#define EOM_CONF_LOCKDOWN_CAN_PRINT             "disable-printing"
#define EOM_CONF_LOCKDOWN_CAN_SETUP_PAGE        "disable-print-setup"

#define EOM_CONF_VIEW_BACKGROUND_COLOR          "background-color"
#define EOM_CONF_VIEW_INTERPOLATE               "interpolate"
#define EOM_CONF_VIEW_EXTRAPOLATE               "extrapolate"
#define EOM_CONF_VIEW_SCROLL_WHEEL_ZOOM         "scroll-wheel-zoom"
#define EOM_CONF_VIEW_ZOOM_MULTIPLIER           "zoom-multiplier"
#define EOM_CONF_VIEW_AUTOROTATE                "autorotate"
#define EOM_CONF_VIEW_TRANSPARENCY              "transparency"
#define EOM_CONF_VIEW_TRANS_COLOR               "trans-color"
#define EOM_CONF_VIEW_USE_BG_COLOR              "use-background-color"

#define EOM_CONF_FULLSCREEN_RANDOM              "random"
#define EOM_CONF_FULLSCREEN_LOOP                "loop"
#define EOM_CONF_FULLSCREEN_UPSCALE             "upscale"
#define EOM_CONF_FULLSCREEN_SECONDS             "seconds"

#define EOM_CONF_UI_TOOLBAR                     "toolbar"
#define EOM_CONF_UI_STATUSBAR                   "statusbar"
#define EOM_CONF_UI_IMAGE_COLLECTION            "image-collection"
#define EOM_CONF_UI_IMAGE_COLLECTION_POSITION   "image-collection-position"
#define EOM_CONF_UI_IMAGE_COLLECTION_RESIZABLE  "image-collection-resizable"
#define EOM_CONF_UI_SIDEBAR                     "sidebar"
#define EOM_CONF_UI_SCROLL_BUTTONS              "scroll-buttons"
#define EOM_CONF_UI_DISABLE_CLOSE_CONFIRMATION	"disable-close-confirmation"
#define EOM_CONF_UI_DISABLE_TRASH_CONFIRMATION  "disable-trash-confirmation"
#define EOM_CONF_UI_FILECHOOSER_XDG_FALLBACK    "filechooser-xdg-fallback"
#define EOM_CONF_UI_PROPSDIALOG_NETBOOK_MODE    "propsdialog-netbook-mode"
#define EOM_CONF_UI_EXTERNAL_EDITOR             "external-editor"

#define EOM_CONF_PLUGINS_ACTIVE_PLUGINS         "active-plugins"

#endif /* __EOM_CONFIG_KEYS_H__ */
