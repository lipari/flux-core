AM_CFLAGS = \
	@GCCWARN@ \
	$(CODE_COVERAGE_CFLAGS)

AM_LDFLAGS = \
	$(CODE_COVERAGE_LDFLAGS)

AM_CPPFLAGS = \
	-I$(top_srcdir) -I$(top_srcdir)/src/include \
	$(ZMQ_CFLAGS)

noinst_LTLIBRARIES = libjsc.la 
fluxcoreinclude_HEADERS = jstatctl.h

libjsc_la_SOURCES = \
	jstatctl.c \
	jstatctl_deprecated.h
