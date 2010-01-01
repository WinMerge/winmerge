#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Date:   Mon Oct 07, 2002 01:56
#
# Copyright (C) 2002-2009 Aleix Conchillo Flaque
#

#### boilerplate and flags

define([scew_version], [1.1.1])

AC_INIT(scew/scew.h, scew_version)
AM_INIT_AUTOMAKE(scew, scew_version)
AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC

# Next macro is deprecated and should be substituted in
# future releases by LT_INIT([win32-dll]).
# Packages in Debian 5.0 still do not support it.
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AM_PROG_CC_C_O

#### options

AC_ARG_ENABLE([debug],
   AC_HELP_STRING([--enable-debug],
      [turn on debug support @<:@default=no@:>@]),
      [], [enable_debug=no])

AC_ARG_ENABLE([threads],
   AC_HELP_STRING([--enable-threads],
      [turn on basic thread support @<:@default=yes@:>@]),
      [], [enable_threads=yes])

# By now, we do not support UTF-16 in Unix systems.
enable_utf16=no
#AC_ARG_ENABLE([utf16],
#   AC_HELP_STRING([--enable-utf16],
#      [turn on UTF-16 support (wchar_t) @<:@default=no@:>@]),
#      [], [enable_utf16=no])

# doxygen

SCEW_DOXYGEN()

AM_CONDITIONAL(DOC, test x$enable_doc = xyes)

#### libraries

AC_CHECK_HEADER(expat.h, ,
                AC_MSG_ERROR(Unable to find Expat library.
                Download lastest version at http://www.libexpat.org))

if test "x$enable_utf16" = "xno"; then
   AC_CHECK_LIB(expat, XML_Parse, ,
                AC_MSG_ERROR(Unable to find Expat library.
                Download lastest version at http://www.libexpat.org))
else
   AC_CHECK_LIB(expatw, XML_Parse, ,
                AC_MSG_ERROR(Unable to find Expat library (UTF-16).
                Download lastest version at http://www.libexpat.org))
fi

if test "x$enable_threads" = "xyes"; then
   AC_CHECK_LIB(pthread, pthread_key_create, ,
                AC_MSG_ERROR(Unable to find pthread libray.))
fi

#### Unit testing framework

PKG_CHECK_MODULES([CHECK], [check >= 0.9.0],
                  [enable_tests=yes], [enable_tests=no])

AM_CONDITIONAL(CHECK_TESTS, test x$enable_tests = xyes)

if test "x$enable_tests" = "xno"; then
   AC_MSG_WARN(Check unit testing framework not found.)
fi

####

CFLAGS="$CFLAGS -Wall -Werror"

# By now, we do not support UTF-16 in Unix systems (see above).
AM_CONDITIONAL(SCEW_UNICODE_WCHAR_T, test x$enable_utf16 = xyes)
if test "x$enable_utf16" = "xyes"; then
   CFLAGS="$CFLAGS -std=c99"
   CPPFLAGS="$CPPFLAGS -DXML_UNICODE_WCHAR_T"
fi

if test "x$enable_debug" = "xno"; then
   CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

AC_SUBST(CFLAGS)

####

AC_OUTPUT(
Makefile \
doc/Doxyfile \
doc/Makefile \
etc/Makefile \
etc/autoconf/Makefile \
etc/pkg-config/scew.pc \
etc/pkg-config/Makefile \
win32/Makefile \
scew/Makefile \
examples/Makefile \
examples/scew_print/Makefile \
examples/scew_stream/Makefile \
examples/scew_write/Makefile
examples/win32/Makefile
examples/win32/scew_print/Makefile \
examples/win32/scew_stream/Makefile \
examples/win32/scew_write/Makefile
tests/Makefile \
tests/data/Makefile \
)

if test "x$enable_debug" = "xyes"; then
   mode=debug
else
   mode=release
fi

echo
echo "*** $PACKAGE $VERSION has been successfully configured ***"
echo
echo "Type 'make' to complete the compilation ($mode mode) and"
echo "then 'make install' as root to install."
echo
echo "A set of examples found in the 'examples' directory will"
echo "also be built. See 'examples/README' to see a list of them."
echo
echo "If you want to run the test suite, type 'make check' (Check"
echo "Unit testing framework needed)."
echo
echo "If you have doxygen installed, type 'make doc' to generate"
echo "the documentation."
echo
echo "Run './configure --help' to see all available options."
echo
