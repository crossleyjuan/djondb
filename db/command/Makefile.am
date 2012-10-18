AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util -I$(srcdir)/../bson -I$(srcdir)/../filesystem -I$(srcdir)/../db  

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-command.la

libdjon_command_la_LDFLAGS = -static# -fPIC
libdjon_command_la_SOURCES = command.cpp \
									  commandreader.cpp \
									  commandresult.cpp \
									  commandwriter.cpp \
									  findcommand.cpp \
									  insertcommand.cpp \
									  updatecommand.cpp \
									  command.h \
									  commandreader.h \
									  commandresult.h \
									  commandwriter.h \
									  findcommand.h \
									  insertcommand.h \
									  updatecommand.h \
									  dropnamespacecommand.h \
									  dropnamespacecommand.cpp \
									  shutdowncommand.cpp \
									  showdbscommand.cpp \
									  shownamespacescommand.cpp

TESTS = test-command
check_PROGRAMS = test-command
#test_command_LDFLAGS = $(all_includes)
test_command_LDADD = libdjon-command.la -ldjon-db -ldjon-filesystem -ldjon-cache -ldjon-bson -ldjon-util -lrt -luuid  -lcpptest -lantlr3c
test_command_SOURCES = tests/main.cpp

library_includedir=$(includedir)/djon-command-1.0/djon-command
#library_include_HEADERS = bson.h

djon_command_libincludedir = $(libdir)/djon-command-1.0/include
#nodist_djon_command_libinclude_HEADERS = config.h

