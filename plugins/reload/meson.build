desktopfile = configure_file(input: 'reload.plugin.desktop.in.in', configuration: pkg_info, output: 'reload.plugin.desktop.in')
if get_option('nls')
  i18n.merge_file(
    input: desktopfile,
    output: 'reload.plugin.desktop',
    install: true,
    install_dir: pluginsdir,
    type: 'desktop',
    po_dir: po_dir,
  )
else
  install_data(desktopfile, install_dir: pluginsdir)
endif

shared_library(
  'reload',
  sources: [
    'eom-reload-plugin.h',
    'eom-reload-plugin.c',
  ],
  c_args: plugins_c_args,
  dependencies: all_deps,
  include_directories: [top_inc, src_inc],
  link_with: [eom_links],
  install: true,
  install_dir: pluginsdir
)
