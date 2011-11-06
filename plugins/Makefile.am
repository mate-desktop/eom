AUTOMAKE_OPTIONS = subdir-objects

pluginsdir = $(libdir)/eom/plugins

INCLUDES = \
	-I$(top_srcdir)/src 				\
	$(EOM_CFLAGS) 					\
	$(WARN_CFLAGS)					\
	-DEOM_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

AM_LDFLAGS = -avoid-version -module

plugins_LTLIBRARIES = \
	fullscreen/libfullscreen.la  \
	reload/libreload.la

plugins_in_files = \
	fullscreen/fullscreen.eom-plugin.desktop.in  \
	reload/reload.eom-plugin.desktop.in

# Fullscreen plugin
fullscreen_libfullscreen_la_SOURCES = \
	fullscreen/eom-fullscreen-plugin.h  \
	fullscreen/eom-fullscreen-plugin.c

fullscreen_libfullscreen_la_LIBADD = $(EOM_LIBS)

# Reload plugin
reload_libreload_la_SOURCES = \
	reload/eom-reload-plugin.h  \
	reload/eom-reload-plugin.c

reload_libreload_la_LIBADD = $(EOM_LIBS)

# Statusbar Date Plugin
if HAVE_EXIF
plugins_LTLIBRARIES += statusbar-date/libstatusbar-date.la

statusbar_date_libstatusbar_date_la_SOURCES = \
	statusbar-date/eom-statusbar-date-plugin.h  \
	statusbar-date/eom-statusbar-date-plugin.c

statusbar_date_libstatusbar_date_la_LIBADD = $(EOM_LIBS)
plugins_in_files += statusbar-date/statusbar-date.eom-plugin.desktop.in
endif

# Generate plugin info files

%.eom-plugin: %.eom-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(AM_V_GEN)$(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugins_DATA = $(plugins_in_files:.eom-plugin.desktop.in=.eom-plugin)

DISTCLEANFILES = $(plugins_DATA)
CLEANFILES = $(plugins_DATA)

EXTRA_DIST = $(plugins_in_files)

-include $(top_srcdir)/git.mk
