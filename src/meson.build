fmt_dep = dependency('fmt', required: false)
if not fmt_dep.found()
  fmt_opts = import('cmake').subproject_options()
  fmt_opts.add_cmake_defines({
    'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
    'MASTER_PROJECT': 'OFF',
  })
  fmt_proj = import('cmake').subproject(
    'fmt',
    options: fmt_opts,
    required: false)
  assert(fmt_proj.found(), 'fmtlib is required')
  fmt_dep = fmt_proj.dependency('fmt')
endif

stdplus_deps = [
  fmt_dep,
]

stdplus_srcs = [
  'exception.cpp',
  'signal.cpp',
]

if has_dl
  stdplus_deps += [
    dl_dep,
  ]

  stdplus_srcs += [
    'dl.cpp',
  ]
endif

if has_fd
  stdplus_srcs += [
    'fd/create.cpp',
    'fd/dupable.cpp',
    'fd/impl.cpp',
    'fd/managed.cpp',
    'fd/mmap.cpp',
    'fd/ops.cpp',
  ]
endif

stdplus_lib = library(
  'stdplus',
  stdplus_srcs,
  include_directories: stdplus_headers,
  implicit_include_directories: false,
  dependencies: stdplus_deps,
  version: meson.project_version(),
  install: true)

stdplus_dep = declare_dependency(
  dependencies: stdplus_deps,
  include_directories: stdplus_headers,
  link_with: stdplus_lib)

stdplus_reqs = []
foreach dep : stdplus_deps
  if dep.type_name() == 'pkgconfig'
    stdplus_reqs += dep
  endif
endforeach

import('pkgconfig').generate(
  stdplus_lib,
  description: 'C++ helper utilities',
  version: meson.project_version(),
  requires: stdplus_reqs)

if has_dl
  stdplus_dl_dep = stdplus_dep

  import('pkgconfig').generate(
    stdplus_lib,
    name: 'stdplus-dl',
    description: 'C++ helper utilities',
    version: meson.project_version(),
    requires: stdplus_reqs)
endif

if has_fd
  stdplus_fd_dep = stdplus_dep

  import('pkgconfig').generate(
    stdplus_lib,
    name: 'stdplus-fd',
    description: 'C++ helper utilities',
    version: meson.project_version(),
    requires: stdplus_reqs)
endif

if has_io_uring
  stdplus_io_uring_deps = [
    stdplus_dep,
    stdplus_fd_dep,
    io_uring_dep,
  ]

  stdplus_io_uring_pre = declare_dependency(
    include_directories: stdplus_io_uring_headers,
    dependencies: stdplus_io_uring_deps)

  stdplus_io_uring_lib = library(
    'stdplus-io_uring',
    'io_uring.cpp',
    dependencies: stdplus_io_uring_pre,
    implicit_include_directories: false,
    version: meson.project_version(),
    install: true)

  stdplus_io_uring_dep = declare_dependency(
    dependencies: stdplus_io_uring_pre,
    link_with: stdplus_io_uring_lib)

  stdplus_io_uring_reqs = []
  foreach dep : stdplus_io_uring_deps
    if dep.type_name() == 'pkgconfig'
      stdplus_io_uring_reqs += dep
    endif
  endforeach

  import('pkgconfig').generate(
    stdplus_io_uring_lib,
    description: 'C++ helper utilities',
    version: meson.project_version(),
    requires: stdplus_io_uring_reqs)
endif

if has_gtest
  stdplus_gtest_deps = [
    fmt_dep,
    gtest_dep,
  ]

  stdplus_gtest_pre = declare_dependency(
    include_directories: stdplus_gtest_headers,
    dependencies: stdplus_gtest_deps)

  stdplus_gtest_lib = library(
    'stdplus-gtest',
    'gtest/tmp.cpp',
    dependencies: stdplus_gtest_pre,
    implicit_include_directories: false,
    version: meson.project_version(),
    install: true)

  stdplus_gtest_dep = declare_dependency(
    dependencies: stdplus_gtest_pre,
    link_with: stdplus_gtest_lib)

  stdplus_gtest_reqs = []
  foreach dep : stdplus_gtest_deps
    if dep.type_name() == 'pkgconfig'
      stdplus_gtest_reqs += dep
    endif
  endforeach

  import('pkgconfig').generate(
    stdplus_gtest_lib,
    description: 'C++ helper utilities',
    version: meson.project_version(),
    requires: stdplus_gtest_reqs)

  install_data(
    'run_with_tmp',
    install_mode: 'rwxr-xr-x',
    install_dir: get_option('bindir'))

  meson.override_find_program('run_with_tmp', files('run_with_tmp'))
endif

run_with_tmp = find_program('run_with_tmp', native: true)
