size = 22x22
actioniconsdir = $(pkgdatadir)/icons/hicolor/$(size)/actions
appiconsdir = $(datadir)/icons/hicolor/$(size)/apps

dist_appicons_DATA = \
	apps/eom.png
dist_actionicons_DATA = \
	actions/eom-plugin.png	  	\
	actions/eom-image-collection.png  \
	actions/slideshow-play.png

noinst_DATA = \
	apps/eom.svg		\
	actions/eom-plugin.svg  \
	actions/slideshow-play.svg

EXTRA_DIST = $(noinst_DATA)

-include $(top_srcdir)/git.mk
