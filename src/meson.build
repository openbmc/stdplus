stdplus_headers = include_directories('.')

stdplus_lib = library(
  'stdplus',
  [
    'stdplus/fd/dupable.cpp',
    'stdplus/fd/managed.cpp',
    'stdplus/fd/ops/create.cpp',
    'stdplus/fd/ops/fcntl.cpp',
    'stdplus/fd/sys.cpp',
    'stdplus/signal.cpp',
  ],
  include_directories: stdplus_headers,
  implicit_include_directories: false,
  version: meson.project_version(),
  install: true)

stdplus = declare_dependency(
  include_directories: stdplus_headers,
  link_with: stdplus_lib)

import('pkgconfig').generate(
  name: 'stdplus',
  description: 'C++ helper utilities',
  version: meson.project_version(),
  libraries: stdplus)

install_headers(
  'stdplus/signal.hpp',
  subdir: 'stdplus')

install_headers(
  'stdplus/fd/base.hpp',
  'stdplus/fd/dupable.hpp',
  'stdplus/fd/managed.hpp',
  'stdplus/fd/mock.hpp',
  'stdplus/fd/ops.hpp',
  'stdplus/fd/sys.hpp',
  'stdplus/fd/sys_mock.hpp',
  subdir: 'stdplus/fd')

install_headers(
  'stdplus/fd/ops/create.hpp',
  'stdplus/fd/ops/fcntl.hpp',
  subdir: 'stdplus/fd/ops')

install_headers(
  'stdplus/handle/copyable.hpp',
  'stdplus/handle/managed.hpp',
  subdir: 'stdplus/handle')

install_headers(
  'stdplus/util/cexec.hpp',
  'stdplus/util/string.hpp',
  subdir: 'stdplus/util')
