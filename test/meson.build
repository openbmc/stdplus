gtest = dependency('gtest', main: true, disabler: true, required: false)
gmock = dependency('gmock', disabler: true, required: false)
if not gtest.found() or not gmock.found()
  gtest_proj = import('cmake').subproject(
    'googletest',
    cmake_options: [
      '-DCMAKE_CXX_FLAGS=-Wno-pedantic',
    ],
    required: false)
  if gtest_proj.found()
    gtest = declare_dependency(
      dependencies: [
        dependency('threads'),
        gtest_proj.dependency('gtest'),
        gtest_proj.dependency('gtest_main'),
      ])
    gmock = gtest_proj.dependency('gmock')
  else
    assert(not build_tests.enabled(), 'Googletest is required')
  endif
endif

gtests = [
  'handle/copyable',
  'handle/managed',
  'util/cexec',
  'util/string',
]

if has_fd
  gtests += [
    'fd/dupable',
    'fd/managed',
    'fd/intf',
    'fd/impl',
    'fd/mock',
    'fd/ops',
  ]
else
  warning('Not testing file descriptor feature')
endif

if gtest.found() and gmock.found()
  foreach t : gtests
    test(t, executable(t.underscorify(), t + '.cpp',
                       build_by_default: false,
                       implicit_include_directories: false,
                       dependencies: [stdplus, gtest, gmock]))
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
  catch2_proj = import('cmake').subproject(
    'Catch2',
    cmake_options: [
      '-DBUILD_TESTING=OFF',
      '-DCMAKE_CXX_FLAGS=-Wno-non-virtual-dtor',
    ],
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
  'signal',
  'raw',
]

if catch2.found()
  foreach t : catch2_tests
    test(t, executable(t.underscorify(), t + '.cpp',
                       build_by_default: false,
                       implicit_include_directories: false,
                       dependencies: [stdplus, span_dep, catch2]))
  endforeach
endif
