/* Eye Of Mate - Jobs
 *
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Lucas Rocha <lucasr@gnome.org>
 *
 * Based on evince code (shell/ev-jobs.h) by:
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

#ifndef __EOM_JOBS_H__
#define __EOM_JOBS_H__

#include "eom-list-store.h"
#include "eom-transform.h"
#include "eom-enums.h"

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#ifndef __EOM_IMAGE_DECLR__
#define __EOM_IMAGE_DECLR__
  typedef struct _EomImage EomImage;
#endif

#ifndef __EOM_URI_CONVERTER_DECLR__
#define __EOM_URI_CONVERTER_DECLR__
typedef struct _EomURIConverter EomURIConverter;
#endif

#ifndef __EOM_JOB_DECLR__
#define __EOM_JOB_DECLR__
typedef struct _EomJob EomJob;
#endif
typedef struct _EomJobClass EomJobClass;

typedef struct _EomJobThumbnail EomJobThumbnail;
typedef struct _EomJobThumbnailClass EomJobThumbnailClass;

typedef struct _EomJobLoad EomJobLoad;
typedef struct _EomJobLoadClass EomJobLoadClass;

typedef struct _EomJobModel EomJobModel;
typedef struct _EomJobModelClass EomJobModelClass;

typedef struct _EomJobTransform EomJobTransform;
typedef struct _EomJobTransformClass EomJobTransformClass;

typedef struct _EomJobSave EomJobSave;
typedef struct _EomJobSaveClass EomJobSaveClass;

typedef struct _EomJobSaveAs EomJobSaveAs;
typedef struct _EomJobSaveAsClass EomJobSaveAsClass;

typedef struct _EomJobCopy EomJobCopy;
typedef struct _EomJobCopyClass EomJobCopyClass;

#define EOM_TYPE_JOB		       (eom_job_get_type())
#define EOM_JOB(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB, EomJob))
#define EOM_JOB_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB, EomJobClass))
#define EOM_IS_JOB(obj)	               (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB))
#define EOM_JOB_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), EOM_TYPE_JOB, EomJobClass))

#define EOM_TYPE_JOB_THUMBNAIL	       (eom_job_thumbnail_get_type())
#define EOM_JOB_THUMBNAIL(obj)	       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_THUMBNAIL, EomJobThumbnail))
#define EOM_JOB_THUMBNAIL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB_THUMBNAIL, EomJobThumbnailClass))
#define EOM_IS_JOB_THUMBNAIL(obj)      (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_THUMBNAIL))

#define EOM_TYPE_JOB_LOAD	       (eom_job_load_get_type())
#define EOM_JOB_LOAD(obj)	       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_LOAD, EomJobLoad))
#define EOM_JOB_LOAD_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB_LOAD, EomJobLoadClass))
#define EOM_IS_JOB_LOAD(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_LOAD))

#define EOM_TYPE_JOB_MODEL	       (eom_job_model_get_type())
#define EOM_JOB_MODEL(obj)	       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_MODEL, EomJobModel))
#define EOM_JOB_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB_MODEL, EomJobModelClass))
#define EOM_IS_JOB_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_MODEL))

#define EOM_TYPE_JOB_TRANSFORM	       (eom_job_transform_get_type())
#define EOM_JOB_TRANSFORM(obj)	       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_TRANSFORM, EomJobTransform))
#define EOM_JOB_TRANSFORM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB_TRANSFORM, EomJobTransformClass))
#define EOM_IS_JOB_TRANSFORM(obj)      (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_TRANSFORM))

#define EOM_TYPE_JOB_SAVE              (eom_job_save_get_type())
#define EOM_JOB_SAVE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_SAVE, EomJobSave))
#define EOM_JOB_SAVE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), EOM_TYPE_JOB_SAVE, EomJobSaveClass))
#define EOM_IS_JOB_SAVE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_SAVE))
#define EOM_JOB_SAVE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), EOM_TYPE_JOB_SAVE, EomJobSaveClass))

#define EOM_TYPE_JOB_SAVE_AS           (eom_job_save_as_get_type())
#define EOM_JOB_SAVE_AS(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_SAVE_AS, EomJobSaveAs))
#define EOM_JOB_SAVE_AS_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass), EOM_TYPE_JOB_SAVE_AS, EomJobSaveAsClass))
#define EOM_IS_JOB_SAVE_AS(obj)        (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_SAVE_AS))

#define EOM_TYPE_JOB_COPY	       (eom_job_copy_get_type())
#define EOM_JOB_COPY(obj)	       (G_TYPE_CHECK_INSTANCE_CAST((obj), EOM_TYPE_JOB_COPY, EomJobCopy))
#define EOM_JOB_COPY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),  EOM_TYPE_JOB_COPY, EomJobCopyClass))
#define EOM_IS_JOB_COPY(obj)      (G_TYPE_CHECK_INSTANCE_TYPE((obj), EOM_TYPE_JOB_COPY))


struct _EomJob
{
	GObject  parent;

	GError   *error;
	GMutex   *mutex;
	float     progress;
	gboolean  finished;
};

struct _EomJobClass
{
	GObjectClass parent_class;

	void    (* finished) (EomJob *job);
	void    (* progress) (EomJob *job, float progress);
	void    (*run)       (EomJob *job);
};

struct _EomJobThumbnail
{
	EomJob       parent;
	EomImage    *image;
	GdkPixbuf   *thumbnail;
};

struct _EomJobThumbnailClass
{
	EomJobClass parent_class;
};

struct _EomJobLoad
{
	EomJob        parent;
	EomImage     *image;
	EomImageData  data;
};

struct _EomJobLoadClass
{
	EomJobClass parent_class;
};

struct _EomJobModel
{
	EomJob        parent;
	EomListStore *store;
	GSList       *file_list;
};

struct _EomJobModelClass
{
        EomJobClass parent_class;
};

struct _EomJobTransform
{
	EomJob        parent;
	GList        *images;
	EomTransform *trans;
};

struct _EomJobTransformClass
{
        EomJobClass parent_class;
};

typedef enum {
	EOM_SAVE_RESPONSE_NONE,
	EOM_SAVE_RESPONSE_RETRY,
	EOM_SAVE_RESPONSE_SKIP,
	EOM_SAVE_RESPONSE_OVERWRITE,
	EOM_SAVE_RESPONSE_CANCEL,
	EOM_SAVE_RESPONSE_LAST
} EomJobSaveResponse;

struct _EomJobSave
{
	EomJob    parent;
	GList	 *images;
	guint      current_pos;
	EomImage *current_image;
};

struct _EomJobSaveClass
{
	EomJobClass parent_class;
};

struct _EomJobSaveAs
{
	EomJobSave       parent;
	EomURIConverter *converter;
	GFile           *file;
};

struct _EomJobSaveAsClass
{
	EomJobSaveClass parent;
};

struct _EomJobCopy
{
	EomJob parent;
	GList *images;
	guint current_pos;
	gchar *dest;
};

struct _EomJobCopyClass
{
	EomJobClass parent_class;
};

/* base job class */
GType           eom_job_get_type           (void) G_GNUC_CONST;
void            eom_job_finished           (EomJob          *job);
void            eom_job_run                (EomJob          *job);
void            eom_job_set_progress       (EomJob          *job,
					    float            progress);

/* EomJobThumbnail */
GType           eom_job_thumbnail_get_type (void) G_GNUC_CONST;
EomJob         *eom_job_thumbnail_new      (EomImage     *image);

/* EomJobLoad */
GType           eom_job_load_get_type      (void) G_GNUC_CONST;
EomJob 	       *eom_job_load_new 	   (EomImage        *image,
					    EomImageData     data);

/* EomJobModel */
GType 		eom_job_model_get_type     (void) G_GNUC_CONST;
EomJob 	       *eom_job_model_new          (GSList          *file_list);

/* EomJobTransform */
GType 		eom_job_transform_get_type (void) G_GNUC_CONST;
EomJob 	       *eom_job_transform_new      (GList           *images,
					    EomTransform    *trans);

/* EomJobSave */
GType		eom_job_save_get_type      (void) G_GNUC_CONST;
EomJob         *eom_job_save_new           (GList           *images);

/* EomJobSaveAs */
GType		eom_job_save_as_get_type   (void) G_GNUC_CONST;
EomJob         *eom_job_save_as_new        (GList           *images,
					    EomURIConverter *converter,
					    GFile           *file);

/* EomJobCopy */
GType          eom_job_copy_get_type      (void) G_GNUC_CONST;
EomJob        *eom_job_copy_new           (GList            *images,
					   const gchar      *dest);

G_END_DECLS

#endif /* __EOM_JOBS_H__ */
