gtest = dependency('gtest', main: true, disabler: true, required: build_tests)
gmock = dependency('gmock', disabler: true, required: build_tests)

tests = [
  'signal',
  'handle/copyable',
  'handle/managed',
  'util/cexec',
  'util/string',
  'util/struct',
]

foreach t : tests
  test(t, executable(t.underscorify(), t + '.cpp',
                     implicit_include_directories: false,
                     dependencies: [stdplus, gtest, gmock]))
endforeach

# span might not have a pkg-config. It is header only so just make
# sure we can access the needed symbols from the header.
span_lite = dependency('span-lite', required: false)
has_span_lite = meson.get_compiler('cpp').has_header_symbol(
  'nonstd/span.hpp',
  'nonstd::dynamic_extent',
  dependencies: span_lite,
  required: build_tests)

if has_span_lite
  test('util/struct_span',
       executable('util_struct_span',
                  'util/struct_span.cpp',
                  implicit_include_directories: false,
                  dependencies: [stdplus, gtest, gmock, span_lite]))
endif
