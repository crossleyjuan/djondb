AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../util -I$(srcdir)/../bson -I$(srcdir)/../filesystem -I$(srcdir)/../cache -I$(srcdir)/../command -I$(srcdir)/../db -I$(srcdir)/../tx -I$(srcdir)/third_party/include 

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

lib_LTLIBRARIES = libdjon-tx.la

libdjon_tx_la_LDFLAGS = -static# -fPIC
libdjon_tx_la_SOURCES = basetransaction.cpp \
								stdtransaction.cpp \
								nodenotifier.cpp \
								transactionmanager.cpp \
								txbuffermanager.cpp \
								txbuffer.cpp

TESTS = test-tx testRollback
check_PROGRAMS = test-tx testRollback
test_tx_LDADD = libdjon-tx.la -ldjon-command -ldjon-db -ldjon-filesystem -ldjon-cache -ldjon-bson -ldjon-util -lX11 -lXss -lrt -luuid -lcpptest -lantlr3c -lpthread
test_tx_SOURCES = tests/main.cpp tests/controllertest.cpp

testRollback_LDADD = libdjon-tx.la -ldjon-command -ldjon-db -ldjon-filesystem -ldjon-cache -ldjon-bson -ldjon-util -lX11 -lXss -lrt -luuid -lcpptest -lantlr3c -lboost_system -lboost_filesystem -lpthread
testRollback_SOURCES = tests/testRollback.cpp tests/controllertest.cpp

djon_tx_libincludedir = $(libdir)/djon-tx-1.0/include


