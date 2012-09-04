/*
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

#ifndef _EOM_FILE_CHOOSER_H_
#define _EOM_FILE_CHOOSER_H_

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define EOM_TYPE_FILE_CHOOSER          (eom_file_chooser_get_type ())
#define EOM_FILE_CHOOSER(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_FILE_CHOOSER, EomFileChooser))
#define EOM_FILE_CHOOSER_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_FILE_CHOOSER, EomFileChooserClass))

#define EOM_IS_FILE_CHOOSER(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_FILE_CHOOSER))
#define EOM_IS_FILE_CHOOSER_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_FILE_CHOOSER))
#define EOM_FILE_CHOOSER_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_FILE_CHOOSER, EomFileChooserClass))

typedef struct _EomFileChooser         EomFileChooser;
typedef struct _EomFileChooserClass    EomFileChooserClass;
typedef struct _EomFileChooserPrivate  EomFileChooserPrivate;

struct _EomFileChooser
{
	GtkFileChooserDialog  parent;

	EomFileChooserPrivate *priv;
};

struct _EomFileChooserClass
{
	GtkFileChooserDialogClass  parent_class;
};


GType		 eom_file_chooser_get_type	(void) G_GNUC_CONST;

GtkWidget	*eom_file_chooser_new		(GtkFileChooserAction action);

GdkPixbufFormat	*eom_file_chooser_get_format	(EomFileChooser *chooser);


G_END_DECLS

#endif /* _EOM_FILE_CHOOSER_H_ */
