/*
 * eom-close-confirmation-dialog.h
 * This file is part of eom
 *
 * Author: Marcus Carlson <marcus@mejlamej.nu>
 *
 * Based on gedit code (gedit/gedit-close-confirmation.h) by gedit Team
 *
 * Copyright (C) 2004-2009 GNOME Foundation
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_CLOSE_CONFIRMATION_DIALOG_H__
#define __EOM_CLOSE_CONFIRMATION_DIALOG_H__

#include <glib.h>
#include <gtk/gtk.h>

#include <eom-image.h>

#define EOM_TYPE_CLOSE_CONFIRMATION_DIALOG		(eom_close_confirmation_dialog_get_type ())
#define EOM_CLOSE_CONFIRMATION_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_CLOSE_CONFIRMATION_DIALOG, EomCloseConfirmationDialog))
#define EOM_CLOSE_CONFIRMATION_DIALOG_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), EOM_TYPE_CLOSE_CONFIRMATION_DIALOG, EomCloseConfirmationDialogClass))
#define EOM_IS_CLOSE_CONFIRMATION_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_CLOSE_CONFIRMATION_DIALOG))
#define EOM_IS_CLOSE_CONFIRMATION_DIALOG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_CLOSE_CONFIRMATION_DIALOG))
#define EOM_CLOSE_CONFIRMATION_DIALOG_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj),EOM_TYPE_CLOSE_CONFIRMATION_DIALOG, EomCloseConfirmationDialogClass))

typedef struct _EomCloseConfirmationDialog 		EomCloseConfirmationDialog;
typedef struct _EomCloseConfirmationDialogClass 	EomCloseConfirmationDialogClass;
typedef struct _EomCloseConfirmationDialogPrivate 	EomCloseConfirmationDialogPrivate;

struct _EomCloseConfirmationDialog
{
	GtkDialog parent;

	/*< private > */
	EomCloseConfirmationDialogPrivate *priv;
};

struct _EomCloseConfirmationDialogClass
{
	GtkDialogClass parent_class;
};

G_GNUC_INTERNAL
GType 		 eom_close_confirmation_dialog_get_type		(void) G_GNUC_CONST;

G_GNUC_INTERNAL
GtkWidget	*eom_close_confirmation_dialog_new			(GtkWindow     *parent,
									 GList         *unsaved_documents);
G_GNUC_INTERNAL
GtkWidget 	*eom_close_confirmation_dialog_new_single 		(GtkWindow     *parent,
									 EomImage      *image);

G_GNUC_INTERNAL
const GList	*eom_close_confirmation_dialog_get_unsaved_images	(EomCloseConfirmationDialog *dlg);

G_GNUC_INTERNAL
GList		*eom_close_confirmation_dialog_get_selected_images	(EomCloseConfirmationDialog *dlg);

#endif /* __EOM_CLOSE_CONFIRMATION_DIALOG_H__ */

