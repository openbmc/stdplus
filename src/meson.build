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

# Function2 might not have a pkg-config. It is header only so just make
# sure we can access the needed symbols from the header.
function2_dep = dependency('function2', required: false)
has_function2 = meson.get_compiler('cpp').has_header_symbol(
    'function2/function2.hpp',
    'fu2::unique_function',
    dependencies: function2_dep,
    required: false
)
if not has_function2
    function2_opts = import('cmake').subproject_options()
    function2_opts.add_cmake_defines({'BUILD_TESTING': 'OFF'})
    function2_proj = import('cmake').subproject(
        'function2',
        options: function2_opts,
        required: false
    )
    assert(function2_proj.found(), 'function2 is required')
    if function2_proj.found()
        function2_dep = function2_proj.dependency('function2')
    endif
endif

stdplus_deps = [
  fmt_dep,
  function2_dep,
]

stdplus_srcs = [
  'cancel.cpp',
  'exception.cpp',
  'flags.cpp',
  'handle/copyable.cpp',
  'handle/managed.cpp',
  'hash.cpp',
  'hash/array.cpp',
  'hash/tuple.cpp',
  'net/addr/ip.cpp',
  'numeric/endian.cpp',
  'pinned.cpp',
  'raw.cpp',
  'signal.cpp',
  'str/cat.cpp',
  'str/cexpr.cpp',
  'str/maps.cpp',
  'util/cexec.cpp',
  'zstring.cpp',
  'zstring_view.cpp',
]

if has_fd
  stdplus_srcs += [
    'fd/atomic.cpp',
    'fd/create.cpp',
    'fd/dupable.cpp',
    'fd/fmt.cpp',
    'fd/impl.cpp',
    'fd/line.cpp',
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
  stdplus_dl_deps = [
    stdplus_dep,
    dl_dep,
  ]

  stdplus_dl_pre = declare_dependency(
    include_directories: stdplus_dl_headers,
    dependencies: stdplus_dl_deps)

  stdplus_dl_lib = library(
    'stdplus-dl',
    'dl.cpp',
    dependencies: stdplus_dl_pre,
    implicit_include_directories: false,
    version: meson.project_version(),
    install: true)

  stdplus_dl_dep = declare_dependency(
    dependencies: stdplus_dl_pre,
    link_with: stdplus_dl_lib)

  stdplus_dl_reqs = []
  foreach dep : stdplus_dl_deps
    if dep.type_name() == 'pkgconfig'
      stdplus_dl_reqs += dep
    endif
  endforeach

  import('pkgconfig').generate(
    stdplus_dl_lib,
    description: 'C++ helper utilities',
    version: meson.project_version(),
    requires: stdplus_dl_reqs)
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
