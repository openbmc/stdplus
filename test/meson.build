gtest = dependency('gtest', main: true, disabler: true, required: build_tests)
gmock = dependency('gmock', disabler: true, required: build_tests)

tests = [
  'signal',
  'fd/dupable',
  'fd/managed',
  'fd/ops/open',
  'handle/copyable',
  'handle/managed',
  'util/cexec',
]

foreach t : tests
  test(t, executable(t.underscorify(), t + '.cpp',
                     implicit_include_directories: false,
                     dependencies: [stdplus, gtest, gmock]))
endforeach
