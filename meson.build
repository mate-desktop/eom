project('eom','c',
  version: '1.28.0',
  meson_version: '>=0.59.0',
  license: 'GPLv2+',
)

eom_api_version = '2.20'

cc = meson.get_compiler('c')

i18n = import('i18n')
gnome = import('gnome')

top_inc = include_directories('.')
po_dir = meson.project_source_root() / 'po'
datadir = join_paths(get_option('prefix'), get_option('datadir'))
pkgdatadir = join_paths(get_option('prefix'), get_option('datadir'), 'eom')
bindir = get_option('prefix') / 'bin'
libdir = join_paths(get_option('prefix'), get_option('libdir'))
schemadir = datadir / 'glib-2.0' / 'schemas'
localedir = join_paths(get_option('prefix'), get_option('localedir'))
includedir = join_paths(get_option('prefix'), get_option('includedir'))
pluginsdir = libdir / 'eom' / 'plugins'

# Dependencies
glib_required = '2.52.0'
gtk_required = '3.22.0'
libpeas_required = '1.8.0'

gio = dependency('gio-2.0', version: '>= ' + glib_required)
gio_unix = dependency('gio-unix-2.0', version: '>= ' + glib_required)
gthread = dependency('gthread-2.0', version: '>= ' + glib_required)
gdk = dependency('gdk-3.0')
gdk_pixbuf = dependency('gdk-pixbuf-2.0', version : '>=2.36.5')
glib = dependency('glib-2.0', version: '>= 2.52.0')
gtk = dependency('gtk+-3.0', version : '>= ' + gtk_required)
gtk_unix_print = dependency('gtk+-unix-print-3.0', version : '>= ' + gtk_required)
gmodule = dependency('gmodule-2.0', required : false)
mate_desktop = dependency('mate-desktop-2.0', version : '>=1.17.0')
shared_mime_info = dependency('shared-mime-info', version : '>= 0.20')
libpeas = dependency('libpeas-1.0', version : '>= ' + libpeas_required)
libpeas_gtk = dependency('libpeas-gtk-1.0', version : '>= ' + libpeas_required)
xml = dependency('libxml-2.0', version: '>=2.0')
gobject_introspection = dependency('gobject-introspection-1.0', version : '>= 0.9.3', required : get_option('introspection'))
xmp = dependency('exempi-2.0', version: '>= 1.99.5', required : get_option('xmp'))
exif = dependency('libexif', version: '>= 0.6.14', required : get_option('libexif'))
rsvg = dependency('librsvg-2.0', version: '>= 2.36.2', required : get_option('librsvg'), not_found_message : 'LIBRSVG support disabled, svg files will not scale')

all_deps = [
  gio, gio_unix, gthread, gdk, glib,
  gtk, gtk_unix_print, gmodule, mate_desktop,
  gdk_pixbuf, shared_mime_info, libpeas, libpeas_gtk,
  xml, gobject_introspection, xmp, exif, rsvg,
]

m = cc.find_library('m', required : false)
all_deps += [m]

z = dependency('zlib')
assert(cc.has_function('crc32', dependencies: [z]), 'zlib do not have crc32')
all_deps += [z]

jpeg = dependency('libjpeg', required : get_option('jpeg'))
if jpeg.found() and (
  not cc.has_function('jpeg_destroy_decompress', dependencies: [jpeg])
  or not cc.has_header('jpeglib.h', dependencies: [jpeg])
)
  jpeg = disabler()
  warn('JPEG loader will not be built (JPEG library not found)')
endif
jpeg_has_progressive = false
if jpeg.found()
  if cc.has_function('jpeg_simple_progression', dependencies: [jpeg])
    jpeg_has_progressive = true
  else
    warn('JPEG library does not support progressive saving')
  endif

  jpeg_lib_version = cc.get_define('JPEG_LIB_VERSION', dependencies: [jpeg], prefix: '#include <jpeglib.h>').to_int()
  jpeg_80 = jpeg_lib_version >= 80
endif

assert(jpeg.found() or get_option('jpeg').disabled(), 'Checks for JPEG loader failed. You can build without it by passing -D jpeg=disabled but some programs using GTK+ may not work properly')
all_deps += [jpeg]

cms = dependency('lcms2', required : get_option('cms'))
all_deps += [cms]
gdk_target_has_x11 = gdk.get_variable('targets').split(' ').contains('x11')
if cms.found() or gdk_target_has_x11
  x11 = dependency('x11')
  all_deps += [x11]
endif

package_url = 'https://mate-desktop.org'
pkg_info = configuration_data({
  'PACKAGE_URL': package_url,
  'VERSION': meson.project_version(),
  'GETTEXT_PACKAGE': meson.project_name(),
  'prefix': get_option('prefix'),
  'exec_prefix': get_option('prefix'),
  'libdir': libdir,
  'includedir': includedir,
  'pluginsdir': includedir / 'eom' / 'plugins',
  'EOM_API_VERSION': eom_api_version,
})

if exif.found() or xmp.found()
  pkg_info.set('EOM_DOC_EXIF_START', '')
  pkg_info.set('EOM_DOC_EXIF_STOP', '')
else
  pkg_info.set('EOM_DOC_EXIF_START', '<!--')
  pkg_info.set('EOM_DOC_EXIF_STOP', '-->')
endif

conf = configuration_data()

conf.set_quoted('PACKAGE', meson.project_name())
conf.set_quoted('PACKAGE_URL', package_url)
conf.set_quoted('VERSION', meson.project_version())
version_parts = meson.project_version().split('.')
conf.set('EOM_MAJOR_VERSION', version_parts[0])
conf.set('EOM_MINOR_VERSION', version_parts[1])
conf.set('EOM_MICRO_VERSION', version_parts[2])

conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
if get_option('nls')
  conf.set('ENABLE_NLS', 1)
endif
if cc.has_function('strptime')
  conf.set('HAVE_STRPTIME', 1)
endif
if cc.has_header_symbol('langinfo.h', '_NL_MEASUREMENT_MEASUREMENT', required: false)
  conf.set('HAVE__NL_MEASUREMENT_MEASUREMENT', 1)
endif

if get_option('thumbnailer')
  if get_option('gdk-pixbuf-thumbnailer')
    eom_thumbnailer = find_program('gdk-pixbuf-thumbnailer').full_path()
  else
    magick = dependency('MagickWand', version : '>= 6.2.6')
    if magick.version().version_compare('>= 7')
      conf.set('HAVE_IMAGEMAGICK7', 1)
    else
      conf.set('HAVE_IMAGEMAGICK6', 1)
    endif
    eom_thumbnailer = 'eom-thumbnailer'
  endif
endif

pkg_info.set('EOM_THUMBNAILER', eom_thumbnailer)

# XMP support
if xmp.found()
  conf.set('HAVE_EXEMPI', 1)
endif
# EXIF support
if exif.found()
  conf.set('HAVE_EXIF', 1)
endif
# JPEG support
if jpeg.found()
  conf.set('HAVE_JPEG', 1)
  conf.set('HAVE_LIBJPEG', 1)
endif
# Jpeg Progressive Saving Support
if jpeg_has_progressive
  conf.set('HAVE_PROGRESSIVE_JPEG', 1)
endif
# Little CMS support
if cms.found()
  conf.set('HAVE_LCMS', 1)
endif
# RSVG Support
if rsvg.found()
  conf.set('HAVE_RSVG', 1)
endif
# Enable GObject Introspection
if gobject_introspection.found()
  conf.set('HAVE_INTROSPECTION', 1)
endif

conf_file = configure_file(output : 'config.h', configuration : conf)
add_project_arguments('-DHAVE_CONFIG_H', language : 'c')

summary({
  'EXIF support': exif.found(),
  'XMP support': xmp.found(),
  'JPEG support': jpeg.found(),
  'RSVG support': rsvg.found(),
  'Colour management support': cms.found(),
  'GObject Introspection': gobject_introspection.found(),
  'Native Language support': get_option('nls'),
  'Thumbnailer': eom_thumbnailer,
})

if get_option('nls')
  subdir('po')
endif

if jpeg.found()
  subdir('jpegutils')
endif

subdir('cut-n-paste')
subdir('src')
subdir('man')
subdir('plugins')
subdir('help')
subdir('data')
subdir('doc')

if get_option('thumbnailer')
  subdir('thumbnailer')
endif

meson.add_dist_script('update-changelog.sh')
