if not get_option('gdk-pixbuf-thumbnailer')
  thumbnailer = executable(
    'eom-thumbnailer',
    sources: [
      'eom-thumbnailer.c',
    ],
    dependencies: [all_deps, magick],
    include_directories: top_inc,
    install: true,
  )
endif

thumbnailerdir = datadir / 'thumbnailers'
thumbnailerfile = configure_file(input: 'eom-thumbnailer.thumbnailer.in', configuration: pkg_info, output: 'eom-thumbnailer.thumbnailer')
install_data(thumbnailerfile, install_dir: thumbnailerdir)
