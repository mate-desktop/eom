#ifndef _EOM_SAVE_AS_DIALOG_HELPER_H_
#define _EOM_SAVE_AS_DIALOG_HELPER_H_

#include <gtk/gtk.h>
#include <gio/gio.h>
#include "eom-uri-converter.h"


G_BEGIN_DECLS

G_GNUC_INTERNAL
GtkWidget*    eom_save_as_dialog_new       (GtkWindow *main, GList *images, GFile *base_file);

G_GNUC_INTERNAL
EomURIConverter* eom_save_as_dialog_get_converter (GtkWidget *dlg);


G_END_DECLS

#endif /* _EOM_SAVE_DIALOG_HELPER_H_ */
