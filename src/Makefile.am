if ENABLE_JPEG
jpeg_LIB = $(top_builddir)/jpegutils/libeom-jpegutils.la
endif

toolbar_LIB = $(top_builddir)/cut-n-paste/toolbar-editor/libtoolbareditor.la

noinst_LTLIBRARIES = libeom.la

bin_PROGRAMS = eom

headerdir = $(prefix)/include/eom-@EOM_API_VERSION@/eom
header_DATA = $(INST_H_FILES)

MARSHAL_OUTPUT = 	\
	eom-marshal.h	\
	eom-marshal.c

NOINST_H_FILES =			\
	eom-application-internal.h	\
	eom-session.h			\
	eom-util.h			\
	eom-pixbuf-util.h		\
	eom-preferences-dialog.h	\
	eom-config-keys.h		\
	eom-image-jpeg.h		\
	eom-image-private.h		\
	eom-metadata-sidebar.h		\
	eom-uri-converter.h		\
	eom-metadata-reader.h		\
	eom-metadata-reader-jpg.h	\
	eom-metadata-reader-png.h	\
	eom-save-as-dialog-helper.h	\
	eom-print-image-setup.h         \
	eom-print-preview.h             \
	eom-print.h			\
	eom-plugin-engine.h             \
	eom-close-confirmation-dialog.h	\
	zoom.h

INST_H_FILES =				\
	eom-application.h		\
	eom-application-activatable.h	\
	eom-debug.h			\
	eom-window.h			\
	eom-window-activatable.h	\
	eom-sidebar.h			\
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
	eom-clipboard-handler.h		\
	eom-jobs.h

libeom_c_files =			\
	eom-application.c		\
	eom-application-activatable.c	\
	eom-session.c			\
	eom-debug.c			\
	eom-util.c			\
	eom-pixbuf-util.c		\
	eom-window.c			\
	eom-window-activatable.c	\
	eom-sidebar.c			\
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
	eom-metadata-sidebar.c	\
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
	eom-close-confirmation-dialog.c \
	eom-plugin-engine.c            	\
	eom-clipboard-handler.c		\
	zoom.c

libeom_la_SOURCES =			\
	$(libeom_c_files)		\
	$(BUILT_SOURCES)		\
	$(NOINST_H_FILES)		\
	$(INST_H_FILES)

if HAVE_EXIF
INST_H_FILES +=				\
	eom-exif-util.h

NOINST_H_FILES +=			\
	eom-metadata-details.h

libeom_c_files +=			\
	eom-metadata-details.c  \
	eom-exif-util.c
endif

if HAVE_EXEMPI
# We need to make sure eom-metadata-details.h
# is only listed once in INST_H_FILES
# or the build will break with automake-1.11
if !HAVE_EXIF
NOINST_H_FILES +=				\
	eom-metadata-details.h
endif !HAVE_EXIF
libeom_la_SOURCES +=			\
	eom-metadata-details.c
endif HAVE_EXEMPI

libeom_la_CPPFLAGS =						\
	-I$(srcdir)						\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/jpegutils				\
	-I$(top_srcdir)/cut-n-paste/toolbar-editor		\
	-DG_LOG_DOMAIN=\"EOM\"					\
	-DEOM_DATA_DIR=\""$(pkgdatadir)"\"			\
	-DEOM_LOCALE_DIR=\""$(datadir)/locale"\"		\
	-DEOM_PLUGIN_DIR=\""$(libdir)/eom/plugins"\"		\
	-DLIBDIR=\""$(libdir)"\"

libeom_la_CFLAGS =						\
	$(EOM_CFLAGS)						\
	$(WARN_CFLAGS)

libeom_la_LIBADD = \
	$(EOM_LIBS)

if HAVE_LCMS
libeom_la_CFLAGS += 	\
	$(X11_CFLAGS)

libeom_la_LIBADD += \
	$(X11_LIBS)
endif

eom_SOURCES = \
	main.c \
	eom-resources.c

eom_CFLAGS = 							\
	-I$(top_srcdir)/cut-n-paste/toolbar-editor		\
	$(EOM_CFLAGS)						\
	-DEOM_DATA_DIR=\""$(pkgdatadir)"\"			\
	-DEOM_LOCALE_DIR=\""$(datadir)/locale"\"		\
	$(INTROSPECTION_CFLAGS)

eom_LDADD = 				\
	libeom.la			\
	$(EOM_LIBS)			\
	$(LIBJPEG)			\
	$(toolbar_LIB)			\
	$(jpeg_LIB)			\
	$(INTROSPECTION_LIBS)

BUILT_SOURCES = 			\
	eom-enum-types.c		\
	eom-enum-types.h		\
	eom-resources.c			\
	$(MARSHAL_OUTPUT)

eom_resources_deps = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(top_srcdir)/data --generate-dependencies $(top_srcdir)/src/eom.gresource.xml)

eom-resources.c: $(top_srcdir)/src/eom.gresource.xml $(eom_resources_deps)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) "$<" --target=$@ \
         --sourcedir=$(top_srcdir)/data --c-name _eom --generate-source

eom-enum-types.h: eom-enum-types.h.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN)(cd $(srcdir) && $(GLIB_MKENUMS) --template eom-enum-types.h.template $(INST_H_FILES)) > $@

eom-enum-types.c: eom-enum-types.c.template $(INST_H_FILES) $(GLIB_MKENUMS)
	$(AM_V_GEN)(cd $(srcdir) && $(GLIB_MKENUMS) --template eom-enum-types.c.template $(INST_H_FILES)) > $@

eom-marshal.h: eom-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --header --internal --prefix=eom_marshal > $@

eom-marshal.c: eom-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --body --header --prefix=eom_marshal > $@

EXTRA_DIST = 				\
	eom.gresource.xml		\
	eom-enum-types.h.template	\
	eom-enum-types.c.template	\
	eom-marshal.list

CLEANFILES = $(BUILT_SOURCES)

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Eom-1.0.gir
INTROSPECTION_SCANNER_ARGS = -I$(top_srcdir) --warn-all $(libeom_la_CPPFLAGS)

Eom-1.0.gir: eom
Eom_1_0_gir_INCLUDES = Gtk-3.0
Eom_1_0_gir_FILES = $(INST_H_FILES) $(libeom_c_files) $(BUILT_SOURCES)
Eom_1_0_gir_VERSION = 1.0
Eom_1_0_gir_PROGRAM = $(builddir)/eom
Eom_1_0_gir_NAMESPACE = Eom
Eom_1_0_gir_CFLAGS = $(EOM_CFLAGS)
girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
