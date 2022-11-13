eggheaders = [
  'egg-editable-toolbar.h',
  'egg-toolbars-model.h',
  'egg-toolbar-editor.h',
]

eggmarshalers = gnome.genmarshal('eggmarshalers', sources: ['eggmarshalers.list'], prefix: '_egg_marshal')
eggtypebuiltins = gnome.mkenums_simple('eggtypebuiltins', sources: eggheaders)

src = [
  eggmarshalers,
  eggtypebuiltins,
  'egg-editable-toolbar.c',
  'egg-toolbars-model.c',
  'egg-toolbar-editor.c',
  eggheaders,
]

toolbareditor = static_library(
  'toolbareditor',
  src,
  dependencies: all_deps,
  include_directories: top_inc,
  c_args: ['-DCURSOR_DIR="' + pkgdatadir + '"'],
)

toolbareditor_inc = include_directories('.')
