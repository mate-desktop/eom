actioniconsdir = pkgdatadir / 'icons' / 'hicolor'
appiconsdir = datadir / 'icons' / 'hicolor'

foreach size : ['16x16', '22x22', '24x24', '32x32']
  install_data([
    size / 'actions/eom-image-collection.png',
    size / 'actions/slideshow-play.png',
  ], install_dir: actioniconsdir / size / 'actions')
endforeach

foreach size : ['16x16', '22x22', '32x32']
  install_data([
    size / 'actions/eom-plugin.png',
  ], install_dir: actioniconsdir / size / 'actions')
endforeach

install_data('48x48/actions/slideshow-play.png', install_dir: actioniconsdir / '48x48' / 'actions')

install_data([
  'scalable/actions/eom-image-collection.svg',
  'scalable/actions/eom-plugin.svg',
  'scalable/actions/slideshow-play.svg',
], install_dir: actioniconsdir / 'scalable' / 'actions')

foreach size : ['16x16', '22x22', '24x24', '32x32']
  install_data([
    size / 'apps/eom.png',
  ], install_dir: appiconsdir / size / 'apps')
endforeach

install_data(['scalable/apps/eom.svg'], install_dir: appiconsdir / 'scalable' / 'apps')

gnome.post_install(gtk_update_icon_cache: true)
