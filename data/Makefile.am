SUBDIRS = pixmaps icons

@INTLTOOL_DESKTOP_RULE@

DESKTOP_IN_FILES= eom.desktop.in.in
DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications
desktop_DATA = $(DESKTOP_FILES)

@INTLTOOL_SCHEMAS_RULE@

schemas_in_files = eom.schemas.in
schemasdir = $(MATECONF_SCHEMA_FILE_DIR)
schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)

gladedir = $(pkgdatadir)
glade_DATA = eom-image-properties-dialog.ui \
	     eom-multiple-save-as-dialog.ui \
	     eom-preferences-dialog.ui

uidir = $(datadir)/eom
UI_FILES = \
	eom-ui.xml \
	eom-toolbar.xml

ui_DATA = $(UI_FILES)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eom.pc

gtkrcdir = $(pkgdatadir)
gtkrc_DATA = gtkrc

install-data-local:
if MATECONF_SCHEMAS_INSTALL
	if test -z "$(DESTDIR)" ; then \
	for p in $(schemas_DATA) ; do \
	MATECONF_CONFIG_SOURCE=$(MATECONF_SCHEMA_CONFIG_SOURCE) $(MATECONFTOOL) --makefile-install-rule $(top_builddir)/data/$$p >&1 > /dev/null; \
	done \
	fi
endif

EXTRA_DIST =			\
        $(gtkrc_DATA)		\
        $(glade_DATA)		\
        $(DESKTOP_IN_FILES)	\
        $(schemas_in_files)	\
	$(UI_FILES)

DISTCLEANFILES = 		\
        $(DESKTOP_FILES)        \
        $(schemas_DATA)

-include $(top_srcdir)/git.mk
