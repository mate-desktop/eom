if ENABLE_JPEG
jpeg_LIB = $(top_builddir)/jpegutils/libeom-jpegutils.la
endif

toolbar_LIB = $(top_builddir)/cut-n-paste/toolbar-editor/libtoolbareditor.la

screensaver_LIB = $(top_builddir)/cut-n-paste/totem-screensaver/libtotemscrsaver.la

noinst_LTLIBRARIES = libeom.la

bin_PROGRAMS = eom

headerdir = $(prefix)/include/eom-@EOM_API_VERSION@/eom
header_DATA = $(INST_H_FILES)

MARSHAL_OUTPUT = 	\
	eom-marshal.h	\
	eom-marshal.c

NOINST_H_FILES =			\
	eom-session.h			\
	eom-util.h			\
	eom-pixbuf-util.h		\
	eom-preferences-dialog.h	\
	eom-config-keys.h		\
	eom-image-jpeg.h		\
	eom-image-private.h		\
	eom-uri-converter.h		\
	eom-metadata-reader.h		\
	eom-metadata-reader-jpg.h	\
	eom-metadata-reader-png.h	\
	eom-save-as-dialog-helper.h	\
	eom-print-image-setup.h         \
	eom-print-preview.h             \
	eom-print.h			\
	eom-module.h             	\
	eom-plugin-manager.h            \
	eom-plugin-engine.h             \
	uta.h				\
	eom-close-confirmation-dialog.h	\
	zoom.h

if ENABLE_PYTHON
NOINST_H_FILES += 			\
	eom-python-module.h		\
	eom-python-plugin.h
endif

INST_H_FILES =				\
	eom-application.h		\
	eom-debug.h			\
	eom-window.h			\
	eom-sidebar.h			\
	eom-dialog.h			\
	eom-properties-dialog.h		\
	eom-error-message-area.h	\
	eom-file-chooser.h		\
	eom-statusbar.h			\
	eom-thumb-nav.h			\
	eom-transform.h			\
	eom-image.h			\
	eom-enums.h			\
	eom-image-save-info.h		\
	eom-scroll-view.h		\
	eom-thumb-view.h		\
	eom-list-store.h		\
	eom-thumbnail.h			\
	eom-job-queue.h			\
	eom-jobs.h			\
	eom-plugin.h

libeom_la_SOURCES =			\
	eom-application.c		\
	eom-session.c			\
	eom-debug.c			\
	eom-util.c			\
	eom-pixbuf-util.c		\
	eom-window.c			\
	eom-sidebar.c			\
	eom-dialog.c			\
	eom-preferences-dialog.c	\
	eom-properties-dialog.c		\
	eom-error-message-area.c	\
	eom-file-chooser.c		\
	eom-statusbar.c			\
	eom-thumb-nav.c			\
	eom-transform.c			\
	eom-image.c			\
	eom-image-jpeg.c		\
	eom-image-save-info.c		\
	eom-scroll-view.c		\
	eom-thumb-view.c		\
	eom-list-store.c		\
	eom-thumbnail.c			\
	eom-job-queue.c			\
	eom-jobs.c			\
	eom-uri-converter.c		\
	eom-metadata-reader.c		\
	eom-metadata-reader-jpg.c	\
	eom-metadata-reader-png.c	\
	eom-save-as-dialog-helper.c	\
	eom-print-image-setup.c         \
	eom-print-preview.c           	\
	eom-print.c			\
	eom-module.c            	\
	eom-close-confirmation-dialog.c \
	eom-plugin.c            	\
	eom-plugin-manager.c            \
	eom-plugin-engine.c            	\
	uta.c				\
	zoom.c				\
	$(BUILT_SOURCES)		\
	$(NOINST_H_FILES)		\
	$(INST_H_FILES)

if HAVE_EXIF
INST_H_FILES +=				\
	eom-exif-util.h			\
	eom-exif-details.h
libeom_la_SOURCES +=			\
	eom-exif-util.c			\
	eom-exif-details.c
endif

if ENABLE_PYTHON
libeom_la_SOURCES += 			\
	eom-python-module.c		\
	eom-python-module.h		\
	eom-python-plugin.c		\
	eom-python-plugin.h
endif

if HAVE_EXEMPI
# We need to make sure eom-exif-details.h
# is only listed once in INST_H_FILES
# or the build will break with automake-1.11
if !HAVE_EXIF
INST_H_FILES +=				\
	eom-exif-details.h
endif !HAVE_EXIF
libeom_la_SOURCES +=			\
	eom-exif-details.c
endif HAVE_EXEMPI

libeom_la_CFLAGS =						\
	-I$(top_srcdir)/jpegutils				\
	-I$(top_srcdir)/cut-n-paste/toolbar-editor		\
	-I$(top_srcdir)/cut-n-paste/totem-screensaver		\
	$(EOM_CFLAGS)						\
	$(WARN_CFLAGS)						\
	-DG_LOG_DOMAIN=\"EOM\"					\
	-DEOM_PREFIX=\""${prefix}"\"				\
	-DEOM_DATA_DIR=\""$(pkgdatadir)"\"			\
	-DEOM_LOCALE_DIR=\""$(datadir)/locale"\"		\
	-DEOM_PIXMAPS_DIR=\""$(datadir)/pixmaps/eom"\"		\
	-DEOM_PLUGIN_DIR=\""$(libdir)/eom/plugins"\"

libeom_la_LIBADD = \
	$(EOM_LIBS)

if HAVE_LCMS
libeom_la_CFLAGS += 	\
	$(X11_CFLAGS)

libeom_la_LIBADD += \
	$(X11_LIBS)
endif

if ENABLE_PYTHON
libeom_la_CFLAGS += 			\
	$(NO_STRICT_ALIASING_CFLAGS)	\
	$(PYGTK_CFLAGS)			\
	$(PYTHON_CFLAGS)		\
	$(AM_CFLAGS)

libeom_la_LIBADD += \
	$(top_builddir)/bindings/python/eom.la
endif

libeom_la_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

eom_SOURCES = main.c

eom_CFLAGS = 							\
	-I$(top_srcdir)/cut-n-paste/toolbar-editor		\
	-I$(top_srcdir)/cut-n-paste/totem-screensaver		\
	$(EOM_CFLAGS)						\
	-DEOM_DATA_DIR=\""$(pkgdatadir)"\"			\
	-DEOM_LOCALE_DIR=\""$(datadir)/locale"\"

eom_LDADD = 				\
	libeom.la			\
	$(EOM_LIBS)			\
	$(LIBJPEG)			\
	$(toolbar_LIB)			\
	$(screensaver_LIB)		\
	$(jpeg_LIB)

eom_LDFLAGS = -export-dynamic -no-undefined -export-symbols-regex "^[[^_]].*"

BUILT_SOURCES = 			\
	eom-enum-types.c		\
	eom-enum-types.h		\
	$(MARSHAL_OUTPUT)

eom-enum-types.h: eom-enum-types.h.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN)(cd $(srcdir) && $(GLIB_MKENUMS) --template eom-enum-types.h.template $(INST_H_FILES)) > $@

eom-enum-types.c: eom-enum-types.c.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN)(cd $(srcdir) && $(GLIB_MKENUMS) --template eom-enum-types.c.template $(INST_H_FILES)) > $@

eom-marshal.h: eom-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --header --internal --prefix=eom_marshal > $@

eom-marshal.c: eom-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --body --header --prefix=eom_marshal > $@

EXTRA_DIST = 				\
	eom-enum-types.h.template	\
	eom-enum-types.c.template	\
	eom-marshal.list

if HAVE_DBUS

BUILT_SOURCES += eom-application-service.h

EXTRA_DIST += eom-application-service.xml

eom-application-service.h: eom-application-service.xml
	$(AM_V_GEN)dbus-binding-tool --prefix=eom_application --mode=glib-server --output=eom-application-service.h $<

endif

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
