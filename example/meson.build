examples = [
]

foreach example : examples
  executable(example, example + '.cpp',
             include_directories: includes,
             implicit_include_directories: false,
             link_with: stdplus)
endforeach
