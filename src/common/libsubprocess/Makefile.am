AM_CFLAGS = \
	@GCCWARN@ \
	$(CODE_COVERAGE_CFLAGS)

AM_LDFLAGS = \
	$(CODE_COVERAGE_LDFLAGS)

AM_CPPFLAGS = \
	-I$(top_srcdir) -I$(top_srcdir)/src/include \
	$(ZMQ_CFLAGS)

noinst_LTLIBRARIES = libsubprocess.la

libsubprocess_la_SOURCES = \
	zio.c \
	zio.h \
	subprocess.c \
	subprocess.h


TESTS = \
	test_subprocess.t \
	test_loop.t \
	test_socketpair.t \
	test_zio.t

check_PROGRAMS = \
	$(TESTS)

TEST_EXTENSIONS = .t
T_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
       $(top_srcdir)/config/tap-driver.sh

test_ldadd = \
	$(top_builddir)/src/common/libflux-internal.la \
	$(top_builddir)/src/common/libflux-core.la \
	$(top_builddir)/src/common/libtap/libtap.la

test_cppflags = \
	$(AM_CPPFLAGS) \
	-I$(top_srcdir)/src/common/libtap

test_subprocess_t_SOURCES = test/subprocess.c
test_subprocess_t_CPPFLAGS = $(test_cppflags)
test_subprocess_t_LDADD = $(test_ldadd)

test_socketpair_t_SOURCES = test/socketpair.c
test_socketpair_t_CPPFLAGS = $(test_cppflags)
test_socketpair_t_LDADD = $(test_ldadd)

test_loop_t_SOURCES = test/loop.c
test_loop_t_CPPFLAGS = $(test_cppflags)
test_loop_t_LDADD = $(test_ldadd)

test_zio_t_SOURCES = test/zio.c
test_zio_t_CPPFLAGS = $(test_cppflags)
test_zio_t_LDADD = $(test_ldadd)
