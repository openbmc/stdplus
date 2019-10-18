stdplus_headers = include_directories('.')

fmt_dep = dependency('fmt', required: false)
if not fmt_dep.found()
  fmt_proj = import('cmake').subproject(
    'fmt',
    cmake_options: [
      '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',
      '-DMASTER_PROJECT=OFF'
    ],
    required: false)
  assert(fmt_proj.found(), 'fmtlib is required')
  fmt_dep = fmt_proj.dependency('fmt')
endif

# span might not have a pkg-config. It is header only so just make
# sure we can access the needed symbols from the header.
span_lite_dep = dependency('span-lite', required: false)
has_span_lite = meson.get_compiler('cpp').has_header_symbol(
  'nonstd/span.hpp',
  'nonstd::dynamic_extent',
  dependencies: span_lite_dep,
  required: false)
if not has_span_lite
  span_lite_proj = import('cmake').subproject(
    'span-lite',
    cmake_options: [
    ],
    required: false)
  if span_lite_proj.found()
    span_lite_dep = span_lite_proj.dependency('span-lite')
    has_span_lite = true
  endif
endif

stdplus_deps = [
  fmt_dep,
  span_lite_dep,
]

stdplus_lib = library(
  'stdplus',
  [
    'stdplus/signal.cpp',
  ],
  include_directories: stdplus_headers,
  implicit_include_directories: false,
  dependencies: stdplus_deps,
  version: meson.project_version(),
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
  'stdplus/raw.hpp',
  'stdplus/signal.hpp',
  subdir: 'stdplus')

install_headers(
  'stdplus/handle/copyable.hpp',
  'stdplus/handle/managed.hpp',
  subdir: 'stdplus/handle')

install_headers(
  'stdplus/util/cexec.hpp',
  'stdplus/util/string.hpp',
  subdir: 'stdplus/util')
