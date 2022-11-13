src = [
  'jpegint.h',
  'transupp.h',
]

if jpeg_80
  src += [
    'jpegint-8a.h',
    'transupp-8a.c',
    'transupp-8a.h',
  ]
else
  src += [
    'transupp-6b.c',
    'transupp-6b.h',
  ]
endif

jpegutils = static_library(
  'eom-jpegutils',
  src,
  dependencies: all_deps,
  include_directories: top_inc,
)

jpegutils_inc = include_directories('.')
