actioniconsdir = pkgdatadir / 'icons' / 'hicolor'
appiconsdir = datadir / 'icons' / 'hicolor'

install_data([
  '16x16/actions/eom-image-collection.png',
  '16x16/actions/eom-plugin.png',
  '16x16/actions/slideshow-play.png',
  '22x22/actions/eom-image-collection.png',
  '22x22/actions/eom-plugin.png',
  '22x22/actions/slideshow-play.png',
  '24x24/actions/eom-image-collection.png',
  '24x24/actions/slideshow-play.png',
  '32x32/actions/eom-image-collection.png',
  '32x32/actions/eom-plugin.png',
  '32x32/actions/slideshow-play.png',
  '48x48/actions/slideshow-play.png',
  'scalable/actions/eom-image-collection.svg',
  'scalable/actions/eom-plugin.svg',
  'scalable/actions/slideshow-play.svg',
], install_dir: actioniconsdir, preserve_path: true)

install_data([
  '16x16/apps/eom.png',
  '22x22/apps/eom.png',
  '24x24/apps/eom.png',
  '32x32/apps/eom.png',
  'scalable/apps/eom.svg',
], install_dir: appiconsdir, preserve_path: true)

meson.add_install_script(find_program('gtk-update-icon-cache'), '-f', '-t', appiconsdir, skip_if_destdir: true)
