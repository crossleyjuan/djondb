AC_INIT(djondb, 0.220130106, crossleyjuan@gmail.com)
    AC_CONFIG_AUX_DIR(config)
#    AC_CONFIG_SRCDIR(src/foobar.cpp)

    AM_INIT_AUTOMAKE

AC_ARG_ENABLE(debug,
AS_HELP_STRING([debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_ARG_ENABLE(log,
AS_HELP_STRING([log],
               [enable logging, default: no]),
[case "${enableval}" in
             0) log=0 ;;
             1)  log=1 ;;
             2)  log=2 ;;
             3)  log=3 ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-log]) ;;
esac],
[log=false])

AM_CONDITIONAL(DETAIL, test x"$log" != x"false")

UNAME=$(uname -s)

AM_CONDITIONAL(MAC, test x"$UNAME" == x"Darwin")

AC_ARG_ENABLE(profile,
AS_HELP_STRING([profile],
               [enable profiling, default: no]),
[case "${enableval}" in
             yes) profile=true ;;
             no)  profile=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
esac],
[profile=false])

AM_CONDITIONAL(PROFILE, test x"$profile" = x"true")

# AC_CONFIG_SUBDIRS(dinnerd libdinner hungryhacker vdinner)
AC_CONFIG_HEADERS([config.h])
AM_COND_IF([DEBUG], AC_DEFINE([DEBUG], [1], [Define Debug variable]))
AM_COND_IF([DETAIL], AC_DEFINE_UNQUOTED([DETAIL], $log, [Define log detail 3 - Maximum, 2 - medium, 1 - low, 0 - normal]))
AM_COND_IF([PROFILE], AC_DEFINE([PROFILE], [1], [Define Profile variable]))


# if text -z $CXXFLAGS; then
#	CXXFLAGS='-g -O0'
#fi
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_OUTPUT(Makefile util/Makefile bson/Makefile cache/Makefile filesystem/Makefile db/Makefile command/Makefile network/Makefile service/Makefile driverbase/Makefile daemon/Makefile shell/Makefile)

