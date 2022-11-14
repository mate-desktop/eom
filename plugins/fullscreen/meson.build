desktopfile = configure_file(input: 'fullscreen.plugin.desktop.in.in', configuration: pkg_info, output: 'fullscreen.plugin.desktop.in')
if get_option('nls')
  i18n.merge_file(
    input: desktopfile,
    output: 'fullscreen.plugin',
    install: true,
    install_dir: pluginsdir,
    type: 'desktop',
    po_dir: po_dir,
  )
else
  install_data(desktopfile, install_dir: pluginsdir, rename: ['fullscreen.plugin'])
endif

shared_library(
  'fullscreen',
  sources: [
    'eom-fullscreen-plugin.h',
    'eom-fullscreen-plugin.c',
  ],
  c_args: plugins_c_args,
  dependencies: all_deps,
  include_directories: [top_inc, src_inc],
  link_with: [eom_links],
  install: true,
  install_dir: pluginsdir
)
