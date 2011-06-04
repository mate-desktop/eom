#ifndef _EOM_SCROLL_VIEW_H_
#define _EOM_SCROLL_VIEW_H_

#include <gtk/gtk.h>
#include "eom-image.h"

G_BEGIN_DECLS

typedef struct _EomScrollView EomScrollView;
typedef struct _EomScrollViewClass EomScrollViewClass;
typedef struct _EomScrollViewPrivate EomScrollViewPrivate;

#define EOM_TYPE_SCROLL_VIEW              (eom_scroll_view_get_type ())
#define EOM_SCROLL_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOM_TYPE_SCROLL_VIEW, EomScrollView))
#define EOM_SCROLL_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), EOM_TYPE_SCROLL_VIEW, EomScrollViewClass))
#define EOM_IS_SCROLL_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOM_TYPE_SCROLL_VIEW))
#define EOM_IS_SCROLL_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), EOM_TYPE_SCROLL_VIEW))


struct _EomScrollView {
	GtkGrid base_instance;

	EomScrollViewPrivate *priv;
};

struct _EomScrollViewClass {
	GtkGridClass parent_class;

	void (* zoom_changed) (EomScrollView *view, double zoom);
};

typedef enum {
	EOM_TRANSP_BACKGROUND,
	EOM_TRANSP_CHECKED,
	EOM_TRANSP_COLOR
} EomTransparencyStyle;

GType    eom_scroll_view_get_type         (void) G_GNUC_CONST;
GtkWidget* eom_scroll_view_new            (void);

/* loading stuff */
void     eom_scroll_view_set_image        (EomScrollView *view, EomImage *image);
EomImage* eom_scroll_view_get_image       (EomScrollView *view);

/* general properties */
void     eom_scroll_view_set_scroll_wheel_zoom (EomScrollView *view, gboolean scroll_wheel_zoom);
void     eom_scroll_view_set_zoom_upscale (EomScrollView *view, gboolean upscale);
void     eom_scroll_view_set_zoom_multiplier (EomScrollView *view, gdouble multiplier);
void     eom_scroll_view_set_antialiasing_in (EomScrollView *view, gboolean state);
void     eom_scroll_view_set_antialiasing_out (EomScrollView *view, gboolean state);
void     eom_scroll_view_set_transparency_color (EomScrollView *view, GdkRGBA *color);
void     eom_scroll_view_set_transparency (EomScrollView *view, EomTransparencyStyle style);
gboolean eom_scroll_view_scrollbars_visible (EomScrollView *view);
void	 eom_scroll_view_set_popup (EomScrollView *view, GtkMenu *menu);
void	 eom_scroll_view_set_background_color (EomScrollView *view,
					       const GdkRGBA *color);
void	 eom_scroll_view_override_bg_color (EomScrollView *view,
					    const GdkRGBA *color);
void     eom_scroll_view_set_use_bg_color (EomScrollView *view, gboolean use);
/* zoom api */
void     eom_scroll_view_zoom_in          (EomScrollView *view, gboolean smooth);
void     eom_scroll_view_zoom_out         (EomScrollView *view, gboolean smooth);
void     eom_scroll_view_zoom_fit         (EomScrollView *view);
void     eom_scroll_view_set_zoom         (EomScrollView *view, double zoom);
double   eom_scroll_view_get_zoom         (EomScrollView *view);
gboolean eom_scroll_view_get_zoom_is_min  (EomScrollView *view);
gboolean eom_scroll_view_get_zoom_is_max  (EomScrollView *view);
void     eom_scroll_view_show_cursor      (EomScrollView *view);
void     eom_scroll_view_hide_cursor      (EomScrollView *view);

G_END_DECLS

#endif /* _EOM_SCROLL_VIEW_H_ */


