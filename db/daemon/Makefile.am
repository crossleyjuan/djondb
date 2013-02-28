AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../service -I$(srcdir)/../util

include $(top_srcdir)/common.mk

AM_CFLAGS += $(all_includes)
AM_CPPFLAGS += $(all_includes)

bin_PROGRAMS = djondbd
djondbd_SOURCES = main.cpp
djondbd_LDADD = $(top_builddir)/service/libdjon-service.la $(top_builddir)/network/libdjon-network.la $(top_builddir)/command/libdjon-command.la $(top_builddir)/db/libdjon-db.la $(top_builddir)/filesystem/libdjon-filesystem.la $(top_builddir)/cache/libdjon-cache.la $(top_builddir)/bson/libdjon-bson.la $(top_builddir)/util/libdjon-util.la -lantlr3c
if !MAC
djondbd_LDFLAGS = -static
djondbd_LDADD += -lrt -luuid
endif

