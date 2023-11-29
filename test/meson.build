gtests = {
  'cancel': [stdplus_dep, gtest_main_dep],
  'exception': [stdplus_dep, gtest_main_dep],
  'function_view': [stdplus_dep, gtest_main_dep],
  'handle/copyable': [stdplus_dep, gtest_main_dep],
  'handle/managed': [stdplus_dep, gtest_main_dep],
  'hash': [stdplus_dep, gtest_main_dep],
  'hash/array': [stdplus_dep, gtest_main_dep],
  'hash/tuple': [stdplus_dep, gtest_main_dep],
  'net/addr/ether': [stdplus_dep, gtest_main_dep],
  'net/addr/ip': [stdplus_dep, gtest_main_dep],
  'net/addr/subnet': [stdplus_dep, gtest_main_dep],
  'numeric/endian': [stdplus_dep, gtest_main_dep],
  'numeric/str': [stdplus_dep, gtest_main_dep],
  'pinned': [stdplus_dep, gtest_main_dep],
  'print': [stdplus_dep, gtest_main_dep],
  'raw': [stdplus_dep, gmock_dep, gtest_main_dep],
  'signal': [stdplus_dep, gtest_main_dep],
  'str/buf': [stdplus_dep, gtest_main_dep],
  'str/cat': [stdplus_dep, gtest_main_dep],
  'str/cexpr': [stdplus_dep, gtest_main_dep],
  'str/conv': [stdplus_dep, gmock_dep, gtest_main_dep],
  'str/maps': [stdplus_dep, gmock_dep, gtest_main_dep],
  'util/cexec': [stdplus_dep, gtest_main_dep],
  'variant': [stdplus_dep, gtest_main_dep],
  'zstring': [stdplus_dep, gtest_main_dep],
  'zstring_view': [stdplus_dep, gtest_main_dep],
}

if has_dl
  gtests += {
    'dl': [stdplus_dl_dep, gtest_main_dep],
  }
elif build_tests.allowed()
  error('Not testing libdl feature')
else
  warning('Not testing libdl feature')
endif

if has_fd
  gtests += {
    'fd/dupable': [stdplus_fd_dep],
    'fd/managed': [stdplus_fd_dep],
    'fd/fmt': [stdplus_fd_dep, stdplus_dep, gtest_main_dep],
    'fd/intf': [stdplus_fd_dep],
    'fd/impl': [stdplus_fd_dep],
    'fd/line': [stdplus_fd_dep, stdplus_dep, gmock_dep, gtest_main_dep],
    'fd/mmap': [stdplus_fd_dep, gtest_main_dep],
    'fd/mock': [stdplus_fd_dep, gmock_dep, gtest_main_dep],
    'fd/ops': [stdplus_fd_dep, gtest_main_dep],
  }
  if has_gtest
    gtests += {
      'fd/atomic': [stdplus_fd_dep, stdplus_gtest_dep, gtest_main_dep],
    }
  elif build_tests.allowed()
    error('Not testing fd/atomic feature')
  else
    warning('Not testing fd/atomic feature')
  endif
elif build_tests.allowed()
  error('Not testing file descriptor feature')
else
  warning('Not testing file descriptor feature')
endif

if has_io_uring
  gtests += {
    'io_uring': [stdplus_io_uring_dep, stdplus_dep, gmock_dep, gtest_main_dep],
  }
elif build_tests.allowed()
  error('Not testing io_uring feature')
else
  warning('Not testing io_uring feature')
endif

if has_gtest
  gtests += {
    'gtest/tmp': [stdplus_gtest_dep, gtest_main_dep],
  }
elif build_tests.allowed()
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
  foreach t, deps : gtests
    test(
      t,
      executable(
        t.underscorify(), t + '.cpp',
        build_by_default: false,
        implicit_include_directories: false,
        cpp_args: '-Wno-missing-braces',
        dependencies: deps))
  endforeach
endif
