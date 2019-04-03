stdplus = library(
  'stdplus',
  [
    'stdplus/signal.cpp',
  ],
  include_directories: includes,
  implicit_include_directories: false,
  version: meson.project_version(),
  install: true)

import('pkgconfig').generate(
  name: 'stdplus',
  description: 'C++ helper utilities',
  version: meson.project_version(),
  libraries: stdplus)

install_headers(
  'stdplus/signal.hpp',
  subdir: 'stdplus')

install_headers(
  'stdplus/handle/copyable.hpp',
  'stdplus/handle/managed.hpp',
  subdir: 'stdplus/handle')

install_headers(
  'stdplus/util/cexec.hpp',
  subdir: 'stdplus/util')
