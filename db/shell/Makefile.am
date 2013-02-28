AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

all_includes = -I$(srcdir)/../driverbase -I$(srcdir)/../db -I$(srcdir)/../util -I$(srcdir)/../bson
#-I$(srcdir)/../util -I$(srcdir)/../util 


if DEBUG
   AM_CFLAGS = -g3 -O0
   DB_XXFLAGS = -g3 -O0
else
   AM_CFLAGS = -O2
   DB_XXFLAGS = -O2
endif

if PROFILE
	PF_XXFLAGS = -pg   
endif

AM_CXXFLAGS = ${PF_XXFLAGS} ${DB_XXFLAGS} ${all_includes}

bin_PROGRAMS = djon-shell
djon_shell_SOURCES = shell.cpp \
							linenoise.c

djon_shell_LDADD = $(top_builddir)/service/libdjon-service.la $(top_builddir)/driverbase/libdjon-client.la $(top_builddir)/network/libdjon-network.la $(top_builddir)/command/libdjon-command.la $(top_builddir)/db/libdjon-db.la $(top_builddir)/filesystem/libdjon-filesystem.la $(top_builddir)/cache/libdjon-cache.la $(top_builddir)/bson/libdjon-bson.la $(top_builddir)/util/libdjon-util.la -lv8 -lantlr3c

if !MAC
djon_shell_LDFLAGS = -static
djon_shell_LDADD += -lrt -luuid
endif

