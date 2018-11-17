# Function2 might not have a pkg-config. It is header only so just make
# sure we can access the needed symbols from the header.
function2 = dependency('function2', required: false)
meson.get_compiler('cpp').has_header_symbol(
  'function2/function2.hpp',
  'fu2::unique_function',
  dependencies: function2,
  required: true)

stdplus_deps = [
  function2
]

stdplus_headers = include_directories('.')

stdplus_lib = library(
  'stdplus',
  [
    'stdplus/fd/dupable.cpp',
    'stdplus/fd/managed.cpp',
    'stdplus/internal/sys.cpp',
    'stdplus/signal.cpp',
  ],
  include_directories: stdplus_headers,
  implicit_include_directories: false,
  version: meson.project_version(),
  dependencies: stdplus_deps,
  install: true)

stdplus = declare_dependency(
  dependencies: stdplus_deps,
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
  subdir: 'stdplus/fd')

install_headers(
  'stdplus/future/future.hpp',
  'stdplus/future/promise.hpp',
  'stdplus/future/util.hpp',
  subdir: 'stdplus/future')

install_headers(
  'stdplus/handle/copyable.hpp',
  'stdplus/handle/managed.hpp',
  subdir: 'stdplus/handle')

install_headers(
  'stdplus/internal/sys.hpp',
  subdir: 'stdplus/internal')

install_headers(
  'stdplus/test/sys.hpp',
  subdir: 'stdplus/test')

install_headers(
  'stdplus/util/cexec.hpp',
  subdir: 'stdplus/util')
