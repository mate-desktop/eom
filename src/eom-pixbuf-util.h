#ifndef _EOM_PIXBUF_UTIL_H_
#define _EOM_PIXBUF_UTIL_H_

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>

G_GNUC_INTERNAL
GSList*          eom_pixbuf_get_savable_formats (void);

G_GNUC_INTERNAL
GdkPixbufFormat* eom_pixbuf_get_format_by_suffix (const char *suffix);

G_GNUC_INTERNAL
GdkPixbufFormat* eom_pixbuf_get_format (GFile *file);

G_GNUC_INTERNAL
char*            eom_pixbuf_get_common_suffix (GdkPixbufFormat *format);

#endif /* _EOM_PIXBUF_UTIL_H_ */

