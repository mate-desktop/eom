#ifndef _EOM_TRANSFORM_H_
#define _EOM_TRANSFORM_H_

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#ifndef __EOM_JOB_DECLR__
#define __EOM_JOB_DECLR__
typedef struct _EomJob EomJob;
#endif

typedef enum {
	EOM_TRANSFORM_NONE,
	EOM_TRANSFORM_ROT_90,
	EOM_TRANSFORM_ROT_180,
	EOM_TRANSFORM_ROT_270,
	EOM_TRANSFORM_FLIP_HORIZONTAL,
	EOM_TRANSFORM_FLIP_VERTICAL,
	EOM_TRANSFORM_TRANSPOSE,
	EOM_TRANSFORM_TRANSVERSE
} EomTransformType;

#define EOM_TYPE_TRANSFORM          (eom_transform_get_type ())
#define EOM_TRANSFORM(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), EOM_TYPE_TRANSFORM, EomTransform))
#define EOM_TRANSFORM_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), EOM_TYPE_TRANSFORM, EomTransformClass))
#define EOM_IS_TRANSFORM(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), EOM_TYPE_TRANSFORM))
#define EOM_IS_TRANSFORM_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), EOM_TYPE_TRANSFORM))
#define EOM_TRANSFORM_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), EOM_TYPE_TRANSFORM, EomTransformClass))

/* =========================================

    GObjecat wrapper around an affine transformation

   ----------------------------------------*/

typedef struct _EomTransform EomTransform;
typedef struct _EomTransformClass EomTransformClass;
typedef struct _EomTransformPrivate EomTransformPrivate;

struct _EomTransform {
	GObject parent;

	EomTransformPrivate *priv;
};

struct _EomTransformClass {
	GObjectClass parent_klass;
};

GType         eom_transform_get_type (void) G_GNUC_CONST;

GdkPixbuf*    eom_transform_apply   (EomTransform *trans, GdkPixbuf *pixbuf, EomJob *job);
EomTransform* eom_transform_reverse (EomTransform *trans);
EomTransform* eom_transform_compose (EomTransform *trans, EomTransform *compose);
gboolean      eom_transform_is_identity (EomTransform *trans);

EomTransform* eom_transform_identity_new (void);
EomTransform* eom_transform_rotate_new (int degree);
EomTransform* eom_transform_flip_new   (EomTransformType type /* only EOM_TRANSFORM_FLIP_* are valid */);
#if 0
EomTransform* eom_transform_scale_new  (double sx, double sy);
#endif
EomTransform* eom_transform_new (EomTransformType trans);

EomTransformType eom_transform_get_transform_type (EomTransform *trans);

gboolean         eom_transform_get_affine (EomTransform *trans, cairo_matrix_t *affine);

G_END_DECLS

#endif /* _EOM_TRANSFORM_H_ */


