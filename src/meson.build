stdplus = library(
  'stdplus',
  [
    'stdplus/signal.cpp',
  ],
  include_directories: includes,
  implicit_include_directories: false,
  install: true)

import('pkgconfig').generate(
  name: 'stdplus',
  description: 'C++ helper utilities',
  version: meson.project_version(),
  libraries: [stdplus],
)

install_headers(
  'stdplus/signal.hpp',
  subdir: 'stdplus')
