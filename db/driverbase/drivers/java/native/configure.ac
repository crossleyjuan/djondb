AC_INIT([djon-javadriver], [0.1], [crossleyjuan@gmail.com], [djon], [http://d-jon.com/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
# AC_CONFIG_SUBDIRS(dinnerd libdinner hungryhacker vdinner)

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AM_CONDITIONAL(MAC, test x"$UNAME" == x"Darwin")

AC_CONFIG_HEADERS([config.h])
LT_INIT([disable-static])
AC_PROG_CXX
AC_SUBST([DJON_JAVADRIVER_SO_VERSION], [0:1:0])
AC_SUBST([DJON_JAVADRIVER_API_VERSION], [0.1])
AC_SUBST([LIB_NAME], [djon-javadriver])

# LIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt $X_EXTRA_LIBS -lGL -lGLU -lglut -lm"

#hungryhacker_LDADD = ../libdinner/libdinner.la

#required headers
#AC_CHECK_HEADERS([util.h bson.h],,AC_MSG_ERROR[required header file missing])

############################################################
# Checking the java path

AC_ARG_VAR([JAVA_HOME], [Java Runtime Environment (JRE) location])

AC_ARG_WITH([jdk], AC_MSG_WARN([Configure the JDK]),
				[JDK_HOME=$withval], )

CPPFLAGS_save="$CPPFLAGS"
AS_IF([test -z "$JDK_HOME"], 
		[AS_IF([test -z "$JAVA_HOME"],
				 AC_MSG_ERROR([if you dont have your JAVA_HOME environment variable you will need to especify the location of jdk using --with-jdk parameter]),
				 [JAVA_DIR=$JAVA_HOME])],
				 [JAVA_DIR=$JDK_HOME])
CPPFLAGS="$CPPFLAGS -I$JAVA_DIR/include"

AC_CHECK_HEADER([jni.h], [have_jni=yes])
AS_IF([test X$have_jni != Xyes ], 
		AC_MSG_ERROR([The jni.h was not found please specify the directory where the java include dir is ]))

############################################################
# check djondb_client.h
AC_LANG_PUSH([C++])

CPPFLAGS_save="$CPPFLAGS"
USR_INCLUDE="-I/usr/include -I/usr/include/djondb"
CPPFLAGS="$CPPFLAGS $USR_INCLUDE"
CXXFLAGS="$CPPFLAGS $USR_INCLUDE"
CFLAGS="$CPPFLAGS $USR_INCLUDE"

#AC_CHECK_HEADERS([/usr/include/djondb/djondb_client.h],[have_djondb=yes])
#AC_CHECK_HEADERS([test.h],[have_djondb=yes])
#AS_IF([test X$have_djondb != Xyes ], 
#		AC_MSG_ERROR([The djondb_client.h was not found please specify the directory where djondb include directory is ]))
#AC_LANG_POP
############################################################

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
