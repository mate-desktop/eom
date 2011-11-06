noinst_LTLIBRARIES = \
	eom.la

nodist_eom_la_SOURCES = \
	eom.c

eom_la_LDFLAGS = \
	-module -avoid-version

eom_la_LIBADD = 		\
	$(PYTHON_LIB_LOC)       \
	$(PYTHON_LIBS)		\
	$(PYTHON_EXTRA_LIBS)	\
	$(PYGTK_LIBS)

eom_la_CFLAGS = 					\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	-I$(top_srcdir)/src				\
	-I$(top_builddir)/src				\
	-I$(top_srcdir)/cut-n-paste/toolbar-editor	\
	$(EOM_CFLAGS)					\
	$(NO_STRICT_ALIASING_CFLAGS)			\
	$(PYGTK_CFLAGS)					\
	$(PYTHON_CFLAGS)				\
	$(AM_CFLAGS)

$(top_builddir)/src/eom-enum-types.h:
	$(AM_V_GEN)cd $(top_builddir)/src && $(MAKE) eom-enum-types.h

eom.c: eom.defs eom.override $(top_builddir)/src/eom-enum-types.h
	$(AM_V_GEN)( cd $(srcdir) && $(PYGOBJECT_CODEGEN) \
		--register $(PYGOBJECT_DEFSDIR)/gio-types.defs \
		--register $(PYGTK_DEFSDIR)/pango-types.defs \
		--register $(PYGTK_DEFSDIR)/gdk-types.defs \
		--register $(PYGTK_DEFSDIR)/gtk-types.defs \
		--override $*.override \
		--prefix py$* $(<F) ) > $@

BINDING_EOM_HEADERS_SRCDIR_IN = 			\
	src/eom-application.h				\
	src/eom-window.h				\
	src/eom-dialog.h				\
	src/eom-properties-dialog.h			\
	src/eom-message-area.h				\
	src/eom-statusbar.h				\
	src/eom-thumb-nav.h				\
	src/eom-image.h					\
	src/eom-scroll-view.h				\
	src/eom-thumb-view.h				\
	src/eom-list-store.h				\
	src/eom-job-queue.h				\
	src/eom-jobs.h					\
	src/eom-plugin.h

BINDING_HEADERS_BUILDDIR_IN =

BINDING_EOM_HEADERS_SRCDIR := $(addprefix $(top_srcdir)/,$(BINDING_EOM_HEADERS_SRCDIR_IN))

BINDING_HEADERS_BUILDDIR   := $(addprefix $(top_builddir)/,$(BINDING_HEADERS_BUILDDIR_IN))

regenerate-python-binding:
	$(PYGTK_H2DEF) $(sort $(BINDING_EOM_HEADERS_SRCDIR) $(BINDING_HEADERS_BUILDDIR)) > eom.defs.new

BUILT_SOURCES = \
	eom.c

EXTRA_DIST = 		\
	eom.override	\
	eom.defs

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
