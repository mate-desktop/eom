/* Eye Of Mate - EOM Preferences Dialog
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __EOM_PREFERENCES_DIALOG_H__
#define __EOM_PREFERENCES_DIALOG_H__

#include "eom-image.h"
#include "eom-thumb-view.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _EomPreferencesDialog EomPreferencesDialog;
typedef struct _EomPreferencesDialogClass EomPreferencesDialogClass;
typedef struct _EomPreferencesDialogPrivate EomPreferencesDialogPrivate;

#define EOM_TYPE_PREFERENCES_DIALOG            (eom_preferences_dialog_get_type ())
#define EOM_PREFERENCES_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_PREFERENCES_DIALOG, EomPreferencesDialog))
#define EOM_PREFERENCES_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_PREFERENCES_DIALOG, EomPreferencesDialogClass))
#define EOM_IS_PREFERENCES_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_PREFERENCES_DIALOG))
#define EOM_IS_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  EOM_TYPE_PREFERENCES_DIALOG))
#define EOM_PREFERENCES_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  EOM_TYPE_PREFERENCES_DIALOG, EomPreferencesDialogClass))

struct _EomPreferencesDialog {
	GtkDialog dialog;

	EomPreferencesDialogPrivate *priv;
};

struct _EomPreferencesDialogClass {
	GtkDialogClass parent_class;
};

G_GNUC_INTERNAL
GType	    eom_preferences_dialog_get_type	  (void) G_GNUC_CONST;

G_GNUC_INTERNAL
GtkWidget    *eom_preferences_dialog_get_instance	  (GtkWindow   *parent);

G_END_DECLS

#endif /* __EOM_PREFERENCES_DIALOG_H__ */
