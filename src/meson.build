resources = gnome.compile_resources(
  'eom',
  'eom.gresource.xml',
  c_name: '_eom',
  source_dir: '../data',
)

marshal = gnome.genmarshal('eom-marshal', sources: ['eom-marshal.list'], prefix: 'eom_marshal')

inst_headers = [
  'eom-application.h',
  'eom-application-activatable.h',
  'eom-debug.h',
  'eom-window.h',
  'eom-window-activatable.h',
  'eom-sidebar.h',
  'eom-properties-dialog.h',
  'eom-error-message-area.h',
  'eom-file-chooser.h',
  'eom-statusbar.h',
  'eom-thumb-nav.h',
  'eom-transform.h',
  'eom-image.h',
  'eom-enums.h',
  'eom-image-save-info.h',
  'eom-scroll-view.h',
  'eom-thumb-view.h',
  'eom-list-store.h',
  'eom-thumbnail.h',
  'eom-job-queue.h',
  'eom-clipboard-handler.h',
  'eom-jobs.h',
]

noinst_headers = [
  'eom-application-internal.h',
  'eom-session.h',
  'eom-util.h',
  'eom-pixbuf-util.h',
  'eom-preferences-dialog.h',
  'eom-config-keys.h',
  'eom-image-jpeg.h',
  'eom-image-private.h',
  'eom-metadata-sidebar.h',
  'eom-uri-converter.h',
  'eom-metadata-reader.h',
  'eom-metadata-reader-jpg.h',
  'eom-metadata-reader-png.h',
  'eom-save-as-dialog-helper.h',
  'eom-print-image-setup.h',
  'eom-print-preview.h',
  'eom-print.h',
  'eom-plugin-engine.h',
  'eom-close-confirmation-dialog.h',
  'zoom.h',
]

enum = gnome.mkenums('eom-enum-types', c_template: 'eom-enum-types.c.template', h_template: 'eom-enum-types.h.template', sources: inst_headers)

c_sources = [
  'eom-application.c',
  'eom-application-activatable.c',
  'eom-session.c',
  'eom-debug.c',
  'eom-util.c',
  'eom-pixbuf-util.c',
  'eom-window.c',
  'eom-window-activatable.c',
  'eom-sidebar.c',
  'eom-preferences-dialog.c',
  'eom-properties-dialog.c',
  'eom-error-message-area.c',
  'eom-file-chooser.c',
  'eom-statusbar.c',
  'eom-thumb-nav.c',
  'eom-transform.c',
  'eom-image.c',
  'eom-image-jpeg.c',
  'eom-image-save-info.c',
  'eom-scroll-view.c',
  'eom-thumb-view.c',
  'eom-list-store.c',
  'eom-metadata-sidebar.c',
  'eom-thumbnail.c',
  'eom-job-queue.c',
  'eom-jobs.c',
  'eom-uri-converter.c',
  'eom-metadata-reader.c',
  'eom-metadata-reader-jpg.c',
  'eom-metadata-reader-png.c',
  'eom-save-as-dialog-helper.c',
  'eom-print-image-setup.c',
  'eom-print-preview.c',
  'eom-print.c',
  'eom-close-confirmation-dialog.c',
  'eom-plugin-engine.c',
  'eom-clipboard-handler.c',
  'zoom.c',
]

libeom_src = [
  c_sources,
  enum,
  resources,
  marshal,
  noinst_headers,
  inst_headers,
]

if exif.found()
  inst_headers += ['eom-exif-util.h']
  noinst_headers += ['eom-metadata-details.h']
  libeom_src += [
    'eom-metadata-details.c',
    'eom-exif-util.c',
  ]
endif

if xmp.found()
  noinst_headers += ['eom-metadata-details.h']
  libeom_src += ['eom-metadata-details.c']
endif

cflags = [
 '-DG_LOG_DOMAIN="EOM"',
 '-DEOM_DATA_DIR="' + pkgdatadir + '"',
 '-DEOM_LOCALE_DIR="' + localedir + '"',
 '-DEOM_PLUGIN_DIR="' + (libdir / 'eom' / 'plugins') + '"',
 '-DLIBDIR="' + libdir + '"',
]

include_dirs = [top_inc, toolbareditor_inc]

if jpeg.found()
  include_dirs += [jpegutils_inc]
endif

libeom = static_library(
  'eom',
  libeom_src,
  c_args: cflags,
  dependencies: all_deps,
  include_directories: include_dirs,
)

src_inc = include_directories('.')

install_headers(inst_headers, install_dir: join_paths(includedir, 'eom-' + eom_api_version, 'eom'))

eom_links = [libeom, toolbareditor]

if jpeg.found()
  eom_links += [jpegutils]
endif

bin = executable(
  'eom', ['main.c', resources],
  install: true,
  c_args: cflags,
  dependencies: all_deps,
  link_with: eom_links,
  include_directories: include_dirs,
)

if gobject_introspection.found()
  gir = gnome.generate_gir(
    bin,
    includes: ['Gtk-3.0'],
    sources: [inst_headers, c_sources, enum, resources, marshal],
    include_directories: include_dirs,
    install: true,
    namespace: 'Eom',
    nsversion: '1.0',
  )
endif
