plugins_c_args = ['-DEOM_LOCALEDIR=' + join_paths(datadir / 'locale')]

subdir('fullscreen')
subdir('reload')

if exif.found()
  subdir('statusbar-date')
endif
