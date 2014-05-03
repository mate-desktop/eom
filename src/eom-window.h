/* Eye of Mate - Main Window
 *
 * Copyright (C) 2000-2008 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on code by:
 * 	- Federico Mena-Quintero <federico@gnu.org>
 *	- Jens Finke <jens@gnome.org>
 * Based on evince code (shell/ev-window.c) by:
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_WINDOW_H__
#define __EOM_WINDOW_H__

#include "eom-list-store.h"
#include "eom-image.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _EomWindow EomWindow;
typedef struct _EomWindowClass EomWindowClass;
typedef struct _EomWindowPrivate EomWindowPrivate;

#define EOM_TYPE_WINDOW            (eom_window_get_type ())
#define EOM_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_WINDOW, EomWindow))
#define EOM_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  EOM_TYPE_WINDOW, EomWindowClass))
#define EOM_IS_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_WINDOW))
#define EOM_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  EOM_TYPE_WINDOW))
#define EOM_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  EOM_TYPE_WINDOW, EomWindowClass))

#define EOM_WINDOW_ERROR           (eom_window_error_quark ())

typedef enum {
	EOM_WINDOW_MODE_UNKNOWN,
	EOM_WINDOW_MODE_NORMAL,
	EOM_WINDOW_MODE_FULLSCREEN,
	EOM_WINDOW_MODE_SLIDESHOW
} EomWindowMode;

typedef enum {
	EOM_WINDOW_COLLECTION_POS_BOTTOM,
	EOM_WINDOW_COLLECTION_POS_LEFT,
	EOM_WINDOW_COLLECTION_POS_TOP,
	EOM_WINDOW_COLLECTION_POS_RIGHT
} EomWindowCollectionPos;

//TODO
typedef enum {
	EOM_WINDOW_ERROR_CONTROL_NOT_FOUND,
	EOM_WINDOW_ERROR_UI_NOT_FOUND,
	EOM_WINDOW_ERROR_NO_PERSIST_FILE_INTERFACE,
	EOM_WINDOW_ERROR_IO,
	EOM_WINDOW_ERROR_TRASH_NOT_FOUND,
	EOM_WINDOW_ERROR_GENERIC,
	EOM_WINDOW_ERROR_UNKNOWN
} EomWindowError;

typedef enum {
	EOM_STARTUP_FULLSCREEN         = 1 << 0,
	EOM_STARTUP_SLIDE_SHOW         = 1 << 1,
	EOM_STARTUP_DISABLE_COLLECTION = 1 << 2
} EomStartupFlags;

struct _EomWindow {
	GtkApplicationWindow win;

	EomWindowPrivate *priv;
};

struct _EomWindowClass {
	GtkApplicationWindowClass parent_class;

	void (* prepared) (EomWindow *window);
};

GType         eom_window_get_type  	(void) G_GNUC_CONST;

GtkWidget    *eom_window_new		(EomStartupFlags  flags);

EomWindowMode eom_window_get_mode       (EomWindow       *window);

void          eom_window_set_mode       (EomWindow       *window,
					 EomWindowMode    mode);

GtkUIManager *eom_window_get_ui_manager (EomWindow       *window);

EomListStore *eom_window_get_store      (EomWindow       *window);

GtkWidget    *eom_window_get_view       (EomWindow       *window);

GtkWidget    *eom_window_get_sidebar    (EomWindow       *window);

GtkWidget    *eom_window_get_thumb_view (EomWindow       *window);

GtkWidget    *eom_window_get_thumb_nav  (EomWindow       *window);

GtkWidget    *eom_window_get_statusbar  (EomWindow       *window);

EomImage     *eom_window_get_image      (EomWindow       *window);

void          eom_window_open_file_list	(EomWindow       *window,
					 GSList          *file_list);

gboolean      eom_window_is_empty 	(EomWindow       *window);

void          eom_window_reload_image (EomWindow *window);
GtkWidget    *eom_window_get_properties_dialog (EomWindow *window);
G_END_DECLS

#endif
