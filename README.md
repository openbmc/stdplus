# stdplus

stdplus is a c++ project containing commonly used classes and functions
for the Linux platform. Note the goals of this project are to implement only
the minimum set of features needed by the OpenBMC project and other users. You
may find the included set of functions currently quite sparse.

This project provides the following primitives:
* A [movable](src/stdplus/handle/managed.hpp) and [copyable](src/stdplus/handle/copyable.hpp) RAII helper wrapper which is used for wrapping c-native types that have custom destruction or copy reference logic.
* [C-Style Error Handler](src/stdplus/util/cexec.hpp) that wrap c-style functions which return errnos and negative error values into functions that throw c++ exceptions.
* [Functions](src/stdplus/signal.hpp) for trivially configuring signals without having to do the normal signal set operations from libc

## Dependencies

Test cases require google{test,mock}, valgrind, and lcov.

## Building
For a standard release build, you want something like:
```
./bootstrap.sh
./configure --disable-examples --disable-tests
make
make install
```

For a test / debug build, a typical configuration is
```
./bootstrap.sh
./configure --enable-tests --enable-coverage --enable-valgrind
make
make check
make check-valgrind
make check-code-coverage
```
