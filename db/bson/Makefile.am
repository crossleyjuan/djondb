AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-bson.la
libdjon_bson_la_LDFLAGS = -static
libdjon_bson_la_SOURCES = bsonobj.cpp \
			  bsonarrayobj.cpp \
			  bsonutil.cpp \
			  bsoncontent.cpp \
			  bsonparser.cpp 

library_includedir=$(includedir)/djondb
library_include_HEADERS = bson.h bsonobj.h bsonutil.h bsonparser.h bsondefs.h bsoncontent.h bsonarrayobj.h

TESTS = test-bson
check_PROGRAMS = test-bson test-performance
test_bson_LDADD = libdjon-bson.la $(top_builddir)/util/libdjon-util.la -lcpptest
if !MAC
     test_bson_LDADD += -lrt -luuid
endif

test_bson_SOURCES = tests/main.cpp

test_performance_SOURCES = tests/testperformance.cpp
test_performance_LDADD = libdjon-bson.la $(top_builddir)/util/libdjon-util.la 
if !MAC
     test_performance_LDADD += -lrt -luuid
endif
