ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
SUBDIRS = util bson cache filesystem db command network service driverbase daemon shell

if DEBUG
   AM_CFLAGS = -g3 -O0
   AM_CPPFLAGS = -g3 -O0
   AM_CXXFLAGS = -g3 -O0
else
   AM_CFLAGS = -O2 -m32
   AM_CPPFLAGS = -O2 -m32
   AM_CXXFLAGS = -O2 -m32
endif

#include aminclude.am

EXTRA_LIBS = $(srcdir)/libs

library_includedir=$(includedir)/djondb
library_include_HEADERS=config.h

