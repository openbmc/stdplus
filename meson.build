project(
  'stdplus',
  'cpp',
  version: '0.1',
  meson_version: '>=0.57.0',
  default_options: [
    'warning_level=3',
    'cpp_std=c++20',
    'tests=' + (meson.is_subproject() ? 'disabled' : 'auto'),
    'examples=' + (meson.is_subproject() ? 'false' : 'true'),
  ])

has_dl = false
if not get_option('dl').disabled()
  dl_dep = meson.get_compiler('cpp').find_library('dl', required: false)
  has_dl = meson.get_compiler('cpp').links('''
    #include <dlfcn.h>
    int main() { dlopen("", 0); }
  ''', dependencies: dl_dep)
endif
if get_option('dl').enabled() and not has_dl
  error('libdl support required')
endif

has_fd = false
if not get_option('fd').disabled()
  has_fd = true
endif

has_io_uring = false
if not get_option('io_uring').disabled() and has_fd
  io_uring_dep = dependency('liburing', required: get_option('io_uring'))
  if io_uring_dep.found()
    has_io_uring = true
  endif
endif
if get_option('io_uring').enabled() and not has_io_uring
  error('io_uring support is required')
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

subdir('src')

build_tests = get_option('tests')
build_examples = get_option('examples')

if build_examples
  subdir('example')
endif
if not build_tests.disabled()
  subdir('test')
endif
