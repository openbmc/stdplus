gtest = dependency('gtest', main: true, disabler: true, required: build_tests)
gmock = dependency('gmock', disabler: true, required: build_tests)

tests = [
  'signal',
  'handle/copyable',
  'handle/managed',
]

foreach t : tests
  test(t, executable(t.underscorify(), t + '.cpp',
                     include_directories: includes,
                     implicit_include_directories: false,
                     link_with: stdplus,
                     dependencies: [gtest, gmock]))
endforeach
