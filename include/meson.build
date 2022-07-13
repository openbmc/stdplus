stdplus_headers = include_directories('.')

if has_dl
  install_headers(
    'stdplus/dl.hpp',
    subdir: 'stdplus')
endif

if has_fd
  install_headers(
    'stdplus/fd/create.hpp',
    'stdplus/fd/dupable.hpp',
    'stdplus/fd/gmock.hpp',
    'stdplus/fd/impl.hpp',
    'stdplus/fd/intf.hpp',
    'stdplus/fd/managed.hpp',
    'stdplus/fd/mmap.hpp',
    'stdplus/fd/ops.hpp',
    subdir: 'stdplus/fd')
endif

if has_io_uring
  install_headers(
    'stdplus/io_uring.hpp',
    subdir: 'stdplus')
endif

install_headers(
  'stdplus/cancel.hpp',
  'stdplus/exception.hpp',
  'stdplus/flags.hpp',
  'stdplus/raw.hpp',
  'stdplus/signal.hpp',
  subdir: 'stdplus')

install_headers(
  'stdplus/handle/copyable.hpp',
  'stdplus/handle/managed.hpp',
  subdir: 'stdplus/handle')

install_headers(
  'stdplus/util/cexec.hpp',
  'stdplus/util/string.hpp',
  subdir: 'stdplus/util')
