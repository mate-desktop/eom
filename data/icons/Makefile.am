
actioniconsdir = $(pkgdatadir)/icons/hicolor
appiconsdir = $(datadir)/icons/hicolor

nobase_dist_actionicons_DATA = \
	16x16/actions/eom-image-collection.png \
	16x16/actions/eom-plugin.png \
	16x16/actions/slideshow-play.png \
	22x22/actions/eom-image-collection.png \
	22x22/actions/eom-plugin.png \
	22x22/actions/slideshow-play.png \
	24x24/actions/eom-image-collection.png \
	24x24/actions/slideshow-play.png \
	32x32/actions/eom-image-collection.png \
	32x32/actions/eom-plugin.png \
	32x32/actions/slideshow-play.png \
	48x48/actions/slideshow-play.png
	scalable/actions/eom-image-collection.svg \
	scalable/actions/eom-plugin.svg \
	scalable/actions/slideshow-play.svg

nobase_dist_appicons_DATA = \
	16x16/apps/eom.png \
	22x22/apps/eom.png \
	24x24/apps/eom.png \
	32x32/apps/eom.png \
	scalable/apps/eom.svg

noinst_DATA = \
	16x16/actions/eom-plugin.svg \
	16x16/actions/slideshow-play.svg \
	16x16/apps/eom.svg \
	22x22/actions/eom-plugin.svg \
	22x22/actions/slideshow-play.svg \
	22x22/apps/eom.svg \
	32x32/actions/eom-image-collection.svg \
	32x32/actions/eom-plugin.svg \
	32x32/actions/slideshow-play.svg \
	32x32/apps/eom.svg

EXTRA_DIST = $(noinst_DATA)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

-include $(top_srcdir)/git.mk
