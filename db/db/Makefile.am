AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util -I$(srcdir)/../bson  -I$(srcdir)/../filesystem -I$(srcdir)/../cache -I$(srcdir)/third_party/include 

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-db.la

libdjon_db_la_LDFLAGS = -static# -fPIC
libdjon_db_la_SOURCES = bplusindex.cpp \
								dbcontroller.cpp \
								indexfactory.cpp \
								indexmanager.cpp \
								filterparser.cpp \
								simpleexpression.cpp \
								constantexpression.cpp \
								unaryexpression.cpp \
								expressionresult.cpp \
								binaryexpression.cpp \
								bplusindex.h \
								dbcontroller.h \
								db.h \
								indexfactory.h \
								index.h \
								indexmanager.h \
								simpleexpression.h \
								constantexpression.h \
								unaryexpression.h \
								binaryexpression.h \
								filterparser.h \
								filterdefs.cpp \
								filter_expressionLexer.cpp \
								filter_expressionParser.cpp \
								streammanager.cpp

TESTS = test-db
check_PROGRAMS = test-db
test_db_LDADD = libdjon-db.la -ldjon-filesystem -ldjon-cache -ldjon-bson -ldjon-util -lrt -luuid -lcpptest -lantlr3c
test_db_SOURCES = tests/main.cpp

library_includedir = $(includedir)/djondb
library_include_HEADERS=filterdefs.h
#nodist_djon_db_libinclude_HEADERS = config.h


