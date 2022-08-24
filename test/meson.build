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

add_test_setup(
  'stdplus',
  exe_wrapper: run_with_tmp,
  env: {'TMPTMPL': 'stdplus-test.XXXXXXXXXX'},
  is_default: true)

add_test_setup(
  'valgrind',
  exe_wrapper: [run_with_tmp, 'valgrind'],
  env: {'TMPTMPL': 'stdplus-test.XXXXXXXXXX'})

if gtest_dep.found() and gmock_dep.found()
  foreach t : gtests
    test(
      t,
      executable(
        t.underscorify(), t + '.cpp',
        build_by_default: false,
        implicit_include_directories: false,
        dependencies: gtest_deps))
  endforeach
endif
