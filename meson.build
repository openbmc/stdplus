project(
  'stdplus',
  'cpp',
  version: '0.1',
  meson_version: '>=1.1.1',
  default_options: [
    'warning_level=3',
    'cpp_std=c++23',
    'tests=' + (meson.is_subproject() ? 'disabled' : 'auto'),
    'examples=' + (meson.is_subproject() ? 'false' : 'true'),
  ])

has_dl = false
if get_option('dl').allowed()
  dl_dep = meson.get_compiler('cpp').find_library('dl', required: false)
  has_dl = meson.get_compiler('cpp').links('''
    #include <dlfcn.h>
    int main() { dlopen("", 0); }
  ''', dependencies: dl_dep)
endif
if get_option('dl').allowed() and not has_dl
  error('libdl support required')
endif

has_fd = false
if get_option('fd').allowed()
  has_fd = true
endif

has_io_uring = false
if get_option('io_uring').allowed() and has_fd
  io_uring_dep = dependency('liburing', required: get_option('io_uring'))
  if io_uring_dep.found()
    has_io_uring = true
  endif
endif
if get_option('io_uring').allowed() and not has_io_uring
  error('io_uring support is required')
endif

build_tests = get_option('tests')
has_gtest = false
if build_tests.disabled() or not get_option('gtest').allowed()
  gtest_dep = dependency('gtest', required: false)
  gtest_main_dep = dependency('gtest', main: true, required: false)
  gmock_dep = dependency('gmock', required: false)
  if not gtest_dep.found() or not gmock_dep.found()
    gtest_opts = import('cmake').subproject_options()
    gtest_opts.add_cmake_defines({
      'BUILD_SHARED_LIBS': 'ON',
      'CMAKE_CXX_FLAGS': '-Wno-pedantic',
    })
    gtest_proj = import('cmake').subproject(
      'googletest',
      options: gtest_opts,
      required: false)
    if gtest_proj.found()
      gtest_dep = declare_dependency(
        dependencies: [
          dependency('threads'),
          gtest_proj.dependency('gtest'),
        ])
      gtest_main_dep = declare_dependency(
        dependencies: [
          gtest_dep,
          gtest_proj.dependency('gtest_main'),
        ])
      gmock_dep = gtest_proj.dependency('gmock')
    else
      assert(not build_tests.allowed() and not get_option('gtest').enabled(), 'Googletest is required')
    endif
  endif
  if get_option('gtest').allowed() and gtest_dep.found()
    has_gtest = true
  endif
endif

subdir('include')
if has_dl
  subdir('include-dl')
endif
if has_fd
  subdir('include-fd')
endif
if has_io_uring
  subdir('include-uring')
endif
if has_gtest
  subdir('include-gtest')
endif

subdir('src')

build_examples = get_option('examples')

if build_examples
  subdir('example')
endif
if build_tests.allowed()
  subdir('test')
endif
