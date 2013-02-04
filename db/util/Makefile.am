AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

include $(top_srcdir)/common.mk

lib_LTLIBRARIES = libdjon-util.la
libdjon_util_la_LDFLAGS = -static
if !MAC
   libdjon_util_la_LIBADD = -lrt -luuid
endif
libdjon_util_la_SOURCES = util.cpp \
			  fileutil.cpp \
			  version.cpp \
			  threads.cpp \
			  dtime.cpp \
			  stringfunctions.cpp \
			  errorhandle.cpp \
			  logger.cpp \
			  datetime.cpp \
			  util.h \
			  config.h \
			  datetime.h \
			  errorhandle.h \
			  dtime.h \
			  fileutil.h \
			  logger.h \
			  version.h \
			  stringfunctions.h \
			  threads.h \
			  settings.h \
			  settings.cpp \
			  defs.h \
			  circular_queue.h 

library_includedir=$(includedir)/djondb
library_include_HEADERS=util.h datetime.h errorhandle.h dtime.h fileutil.h logger.h version.h stringfunctions.h threads.h settings.h defs.h circular_queue.h errors.h


TESTS = test-util
check_PROGRAMS = test-util
test_util_LDADD = libdjon-util.la -lcpptest
if !MAC
     test_util_LDADD += -lrt -luuid
endif
test_util_SOURCES = tests/main.cpp

djon_util_libincludedir = $(libdir)/djon-util-1.0/include
# nodist_djon_util_libinclude_HEADERS = config.h


