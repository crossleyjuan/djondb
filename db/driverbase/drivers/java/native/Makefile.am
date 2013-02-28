AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../.. -I/usr/lib/jvm/java-6-openjdk/include

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-javadriver.la
libdjon_javadriver_la_LDFLAGS = -shared -fPIC
libdjon_javadriver_la_SOURCES = javadriver_wrap.cpp

#libdjon_javadriver_la_LIBADD = -ldjon-client -ldjon-command -ldjon-db -ldjon-filesystem -ldjon-cache -ldjon-bson -ldjon-util -lX11 -lXss -lrt /usr/lib/x86_64-linux-gnu/libuuid.a  -lantlr3c  -lcpptest -lstdc++
libdjon_javadriver_la_LIBADD = -ldjon-client -lX11 -lXss -lrt -luuid  -lantlr3c  -lcpptest -lstdc++

library_includedir=$(includedir)/djon-javadriver-1.0/djon-javadriver
# library_include_HEADERS = Connection.h

TESTS = test-javadriver
bin_PROGRAMS = test-javadriver
check_PROGRAMS = test-javadriver
test_javadriver_LDADD = libdjon-javadriver.la -ldjon-client -lX11 -lXss -lrt -luuid   -lcpptest
test_javadriver_SOURCES = tests/main.cpp

djon_bson_libincludedir = $(libdir)/djon-bson-1.0/include
#nodist_djon_bson_libinclude_HEADERS = config.h
