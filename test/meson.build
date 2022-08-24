gtests = [
  'cancel',
  'exception',
  'handle/copyable',
  'handle/managed',
  'raw',
  'signal',
  'util/cexec',
  'util/string',
]

gtest_deps = [
  stdplus_dep,
  gtest_main_dep,
  gmock_dep,
]

if has_dl
  gtests += [
    'dl',
  ]

  gtest_deps += [
    stdplus_dl_dep,
  ]
elif build_tests.enabled()
  error('Not testing libdl feature')
else
  warning('Not testing libdl feature')
endif

if has_fd
  gtests += [
    'fd/dupable',
    'fd/managed',
    'fd/fmt',
    'fd/intf',
    'fd/impl',
    'fd/line',
    'fd/mmap',
    'fd/mock',
    'fd/ops',
  ]

  gtest_deps += [
    stdplus_fd_dep,
  ]
elif build_tests.enabled()
  error('Not testing file descriptor feature')
else
  warning('Not testing file descriptor feature')
endif

if has_io_uring
  gtests += [
    'io_uring',
  ]

  gtest_deps += [
    stdplus_io_uring_dep,
  ]
elif build_tests.enabled()
  error('Not testing io_uring feature')
else
  warning('Not testing io_uring feature')
endif

if has_gtest
  gtests += [
    'gtest/tmp',
  ]

  gtest_deps += [
    stdplus_gtest_dep,
  ]
elif build_tests.enabled()
  error('Not testing gtest lib feature')
else
  warning('Not testing gtest lib feature')
endif

if gtest_dep.found() and gmock_dep.found()
  foreach t : gtests
    test(
      t,
      run_with_tmp,
      env: {'TMPTMPL': 'stdplus-test.XXXXXXXXXX'},
      args: executable(
        t.underscorify(), t + '.cpp',
        build_by_default: false,
        implicit_include_directories: false,
        dependencies: gtest_deps))
  endforeach
endif

# Switch to `catch2-with-main.pc` when 2.x supports it
catch2 = dependency('catch2-with-main', required: false)
if not catch2.found()
  catch2 = dependency('catch2', required: false)
  if catch2.found()
    catch2_main = meson.get_compiler('cpp').find_library(
      'Catch2WithMain', required: false)
    if catch2_main.found()
      catch2 = declare_dependency(
        dependencies: [
          catch2,
          catch2_main,
        ])
    else
      catch2 = declare_dependency(
        link_with: static_library(
          'catch2', 'catch2_main.cpp',
          implicit_include_directories: false,
          build_by_default: false,
          dependencies: catch2),
        dependencies: catch2)
    endif
  endif
endif
if not catch2.found()
  catch2_opts = import('cmake').subproject_options()
  catch2_opts.add_cmake_defines({
    'BUILD_TESTING': 'OFF',
    'CMAKE_CXX_FLAGS': '-Wno-non-virtual-dtor',
  })
  catch2_proj = import('cmake').subproject(
    'Catch2',
    options: catch2_opts,
    required: false)
  if catch2_proj.found()
    catch2 = declare_dependency(
      dependencies: [
        catch2_proj.dependency('Catch2'),
        catch2_proj.dependency('Catch2WithMain'),
      ])
  else
    assert(not build_tests.enabled(), 'Catch2 is required')
  endif
endif

catch2_tests = [
]

if catch2.found()
  foreach t : catch2_tests
    test(t, executable(t.underscorify(), t + '.cpp',
                       build_by_default: false,
                       implicit_include_directories: false,
                       dependencies: [stdplus_dep, catch2]))
  endforeach
endif
