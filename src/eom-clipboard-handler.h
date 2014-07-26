/*
 * eom-clipboard-handler.h
 * This file is part of eom
 *
 * Author: Felix Riemann <friemann@gnome.org>
 *
 * Copyright (C) 2010 GNOME Foundation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __EOM_CLIPBOARD_HANDLER_H__
#define __EOM_CLIPBOARD_HANDLER_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include "eom-image.h"

G_BEGIN_DECLS

#define EOM_TYPE_CLIPBOARD_HANDLER          (eom_clipboard_handler_get_type ())
#define EOM_CLIPBOARD_HANDLER(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_CLIPBOARD_HANDLER, EomClipboardHandler))
#define EOM_CLIPBOARD_HANDLER_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_CLIPBOARD_HANDLER, EomClipboardHandlerClass))
#define EOM_IS_CLIPBOARD_HANDLER(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_CLIPBOARD_HANDLER))
#define EOM_IS_CLIPBOARD_HANDLER_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_CLIPBOARD_HANDLER))
#define EOM_CLIPBOARD_HANDLER_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_CLIPBOARD_HANDLER, EomClipboardHandlerClass))

typedef struct _EomClipboardHandler EomClipboardHandler;
typedef struct _EomClipboardHandlerClass EomClipboardHandlerClass;
typedef struct _EomClipboardHandlerPrivate EomClipboardHandlerPrivate;

struct _EomClipboardHandler {
	GObject parent;

	EomClipboardHandlerPrivate *priv;
};

struct _EomClipboardHandlerClass {
	GObjectClass parent_klass;
};

GType eom_clipboard_handler_get_type (void) G_GNUC_CONST;

EomClipboardHandler* eom_clipboard_handler_new (EomImage *img);

void eom_clipboard_handler_copy_to_clipboard (EomClipboardHandler *handler,
					      GtkClipboard *clipboard);

G_END_DECLS
#endif /* __EOM_CLIPBOARD_HANDLER_H__ */
