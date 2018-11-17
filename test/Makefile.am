@VALGRIND_CHECK_RULES@
@CODE_COVERAGE_RULES@

gtest_cppflags = $(AM_CPPFLAGS) $(GTEST_CFLAGS) $(GMOCK_CFLAGS)
gtest_ldadd = $(STDPLUS_LIBS) $(GTEST_LIBS) $(GMOCK_LIBS) -lgmock_main

check_PROGRAMS =
TESTS = $(check_PROGRAMS)

check_PROGRAMS += handle/copyable
handle_copyable_SOURCES = handle/copyable.cpp
handle_copyable_CPPFLAGS = $(gtest_cppflags)
handle_copyable_LDADD = $(gtest_ldadd)

check_PROGRAMS += handle/managed
handle_managed_SOURCES = handle/managed.cpp
handle_managed_CPPFLAGS = $(gtest_cppflags)
handle_managed_LDADD = $(gtest_ldadd)

check_PROGRAMS += signal
signal_SOURCES = signal.cpp
signal_CPPFLAGS = $(gtest_cppflags)
signal_LDADD = $(gtest_ldadd)

check_PROGRAMS += util/cexec
util_cexec_SOURCES = util/cexec.cpp
util_cexec_CPPFLAGS = $(gtest_cppflags)
util_cexec_LDADD = $(gtest_ldadd)
