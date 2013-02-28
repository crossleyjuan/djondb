AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util -I$(srcdir)/../bson -I$(srcdir)/../filesystem -I$(srcdir)/../command -I$(srcdir)/../db

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-network.la

libdjon_network_la_LDFLAGS = -static# -fPIC
libdjon_network_la_SOURCES = networkservice.cpp \
                           network.h \
                           networkservice.h

library_includedir=$(includedir)/djon-network-1.0/djon-network
library_include_HEADERS = network.h

TESTS = test-network
check_PROGRAMS = test-network
test_network_LDADD = libdjon-network.la -ldjon-command -ldjon-filesystem -ldjon-util -ldjon-bson -ldjon-db -ldjon-cache -lrt -luuid -lcpptest
test_network_SOURCES = tests/main.cpp

djon_network_libincludedir = $(libdir)/djon-network-1.0/include
#nodist_djon_network_libinclude_HEADERS = config.h



