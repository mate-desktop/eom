SUBDIRS = pixmaps icons

@INTLTOOL_DESKTOP_RULE@

DESKTOP_IN_FILES= eom.desktop.in.in
DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications
desktop_DATA = $(DESKTOP_FILES)

@INTLTOOL_XML_NOMERGE_RULE@
gsettings_SCHEMAS = org.mate.eom.gschema.xml
@GSETTINGS_RULES@

%.gschema.xml.in: %.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@datadir\@^$(datadir)^g' -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

convertdir = $(datadir)/MateConf/gsettings
convert_DATA = eom.convert

gladedir = $(pkgdatadir)
glade_DATA = eom-image-properties-dialog.ui \
	     eom-multiple-save-as-dialog.ui \
	     eom-preferences-dialog.ui

uidir = $(pkgdatadir)
UI_FILES = \
	eom-ui.xml \
	eom-toolbar.xml

ui_DATA = $(UI_FILES)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eom.pc

gtkrcdir = $(pkgdatadir)
gtkrc_DATA = gtkrc

EXTRA_DIST =				\
        $(gtkrc_DATA)			\
        $(glade_DATA)			\
	$(convert_DATA)			\
        $(DESKTOP_IN_FILES)		\
        $(gsettings_SCHEMAS).in.in	\
	$(UI_FILES)

DISTCLEANFILES = 		\
        $(DESKTOP_FILES)	\
        $(gsettings_SCHEMAS_in)	\
	$(gsettings_SCHEMAS)

-include $(top_srcdir)/git.mk
