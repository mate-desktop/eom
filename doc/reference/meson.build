ignore_headers = [
  'eom-image-private.h',
  'eom-image-jpeg.h',
  'eom-marshal.h',
  'eom-application-service.h',
  'eom-enum-types.h',
]

if not exif.found()
  ignore_headers += [
    'eom-exif-util.h',
    'eom-metadata-details.h',
  ]
endif

sgml = configure_file(input: 'eom-docs.sgml.in', configuration: pkg_info, output: 'eom-docs.sgml')
versionsxml = configure_file(input: 'version.xml.in', configuration: pkg_info, output: 'version.xml')

if false
gnome.gtkdoc(
  'eom',
  src_dir: src_inc,
  include_directories: include_dirs,
  main_sgml: 'eom-docs.sgml.in',
  scan_args: ['--rebuild-types'],
  mkdb_args: ['--sgml-mode', '--output-format=xml'],
  ignore_headers: ignore_headers,
  content_files: [versionsxml],
  dependencies: [all_deps, libeom],
  install: true,
)
endif
