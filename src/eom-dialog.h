/* Eye Of Mate - EOM Dialog
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
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

#ifndef __EOM_DIALOG_H__
#define __EOM_DIALOG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _EomDialog EomDialog;
typedef struct _EomDialogClass EomDialogClass;
typedef struct _EomDialogPrivate EomDialogPrivate;

#define EOM_TYPE_DIALOG            (eom_dialog_get_type ())
#define EOM_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_DIALOG, EomDialog))
#define EOM_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_DIALOG, EomDialogClass))
#define EOM_IS_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_DIALOG))
#define EOM_IS_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  EOM_TYPE_DIALOG))
#define EOM_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_DIALOG, EomDialogClass))

struct _EomDialog {
	GObject object;

	EomDialogPrivate *priv;
};

struct _EomDialogClass {
	GObjectClass parent_class;

	void    (* construct)   (EomDialog   *dialog,
				 const gchar *glade_file,
				 const gchar *dlg_node);

	void    (* show)        (EomDialog   *dialog);

	void    (* hide)        (EomDialog   *dialog);
};

GType   eom_dialog_get_type      (void) G_GNUC_CONST;

void    eom_dialog_construct     (EomDialog   *dialog,
				  const gchar *glade_file,
				  const gchar *dlg_node);

void    eom_dialog_show	         (EomDialog *dialog);

void    eom_dialog_hide	         (EomDialog *dialog);

void    eom_dialog_get_controls  (EomDialog   *dialog,
				  const gchar *property_id,
				  ...);

G_END_DECLS

#endif /* __EOM_DIALOG_H__ */
