SUBDIRS = pixmaps icons

desktopdir = $(datadir)/applications
desktop_DATA = eom.desktop
desktop_in_files = $(desktop_DATA:.desktop=.desktop.in)
desktop_in_in_files = $(desktop_in_files:.desktop.in=.desktop.in.in)
$(desktop_DATA): $(desktop_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Name --keyword=Comment --keyword=Keywords --template $< -d $(top_srcdir)/po -o $@

appdatadir = $(datadir)/metainfo
appdata_DATA = eom.appdata.xml
appdata_in_files = $(appdata_DATA:.xml=.xml.in)
appdata_in_in_files = $(appdata_in_files:.xml.in=.xml.in.in)
$(appdata_DATA): $(appdata_in_files)
	$(AM_V_GEN) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@

gsettings_ENUM_NAMESPACE = org.mate.eom
gsettings_ENUM_FILES = $(top_srcdir)/src/eom-scroll-view.h	\
                       $(top_srcdir)/src/eom-window.h

gsettings_SCHEMAS = org.mate.eom.gschema.xml
@GSETTINGS_RULES@

uidir = $(pkgdatadir)
ui_DATA = \
	eom-toolbar.xml

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eom.pc

EXTRA_DIST = \
	$(ui_DATA)		\
	eom.css			\
	eom-image-properties-dialog.ui	\
	eom-multiple-save-as-dialog.ui	\
	eom-preferences-dialog.ui \
	eom-ui.xml		\
	metadata-sidebar.ui	\
	eom.about		\
	$(appdata_in_in_files)	\
	$(desktop_in_in_files)

CLEANFILES = \
	$(appdata_DATA) \
	$(desktop_DATA)

DISTCLEANFILES = \
	$(appdata_in_files) \
	$(desktop_in_files) \
	$(gsettings_SCHEMAS)

-include $(top_srcdir)/git.mk
