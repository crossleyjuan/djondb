AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util -I$(srcdir)/../bson

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-filesystem.la

libdjon_filesystem_la_LDFLAGS = -static
libdjon_filesystem_la_SOURCES = bsoninputstream.cpp \
										  bsonoutputstream.cpp \
										  fileinputoutputstream.cpp \
										  fileinputstream.cpp \
										  fileoutputstream.cpp \
										  networkinputstream.cpp \
										  networkoutputstream.cpp \
										  memorystream.cpp 

TESTS = test-filesystem
check_PROGRAMS = test-filesystem
test_filesystem_LDADD = libdjon-filesystem.la $(top_builddir)/bson/libdjon-bson.la $(top_builddir)/util/libdjon-util.la -lpthread -lcpptest
if !MAC
   test_filesystem_LDADD += -lrt -luuid 
endif
test_filesystem_SOURCES = tests/main.cpp


djon_filesystem_libincludedir = $(libdir)/djon-filesystem-1.0/include
#nodist_djon_filesystem_libinclude_HEADERS = config.h


