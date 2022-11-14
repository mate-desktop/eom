subdir('icons')

desktopdir = datadir / 'applications'
desktopfile = configure_file(input: 'eom.desktop.in.in', configuration: pkg_info, output: 'eom.desktop.in')

if get_option('nls')
  i18n.merge_file(
    input: desktopfile,
    output: 'eom.desktop',
    install: true,
    install_dir: desktopdir,
    type: 'desktop',
    po_dir: po_dir,
  )
else
  install_data(desktopfile, install_dir: desktopdir, rename: ['eom.desktop'])
endif
gnome.post_install(update_desktop_database: true)

appdatadir = join_paths(datadir, 'metainfo')
appdatafile = configure_file(input: 'eom.appdata.xml.in.in', configuration: pkg_info, output: 'eom.appdata.xml.in')

if get_option('nls')
  i18n.merge_file(
    input: appdatafile,
    output: 'eom.appdata.xml',
    install: true,
    install_dir: appdatadir,
    type: 'xml',
    po_dir: po_dir,
  )
else
  install_data(appdatafile, install_dir: appdatadir, rename: ['eom.appdata.xml'])
endif

gsettings_schema = configure_file(input: 'org.mate.eom.gschema.xml.in', configuration: pkg_info, output: 'org.mate.eom.gschema.xml')
install_data(gsettings_schema, install_dir: schemadir)
gnome.post_install(glib_compile_schemas: true)

gnome.mkenums(
  'org.mate.eom.enums.xml',
  comments: '<!-- @comment@ -->',
  sources: enum_headers,
  fhead: '<schemalist>',
  vhead: '''  <@type@ id='org.mate.eom.@EnumName@'>''',
  vprod: '''    <value nick='@valuenick@' value='@valuenum@'/>''',
  vtail: '  </@type@>',
  ftail: '</schemalist>',
  install_header: true,
  install_dir: schemadir
)

eom_pc = configure_file(input: 'eom.pc.in', configuration: pkg_info, output: 'eom.pc')
install_data(eom_pc, install_dir: libdir / 'pkgconfig')

uidir = pkgdatadir
install_data('eom-toolbar.xml', install_dir: uidir)
