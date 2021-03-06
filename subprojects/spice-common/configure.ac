AC_PREREQ([2.63])

AC_INIT([spice-common],
        [noversion],
        [spice-devel@lists.freedesktop.org])

AC_CONFIG_SRCDIR([common/log.h])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([build-aux])

# For automake >= 1.12
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Checks for programs
AM_INIT_AUTOMAKE([1.11 dist-xz no-dist-gzip tar-ustar foreign subdir-objects -Wall -Werror])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])
LT_INIT
SPICE_MANUAL

AC_PROG_CC
AC_PROG_CC_C99
if test "x$ac_cv_prog_cc_c99" = xno; then
    AC_MSG_ERROR([C99 compiler is required.])
fi
AM_PROG_CC_C_O

AC_CHECK_HEADERS([sys/mman.h regex.h])
AC_CHECK_FUNCS([sigaction drand48 setlinebuf])
AC_SEARCH_LIBS(regcomp, [regex rx])

SPICE_CHECK_SYSDEPS
SPICE_EXTRA_CHECKS

AC_ARG_ENABLE([tests],
  AS_HELP_STRING([--enable-tests],
                 [Enable tests @<:@default=yes@:>@]),
  [],
  enable_tests="yes")
AM_CONDITIONAL(ENABLE_TESTS, test "x$enable_tests" = "xyes")

AC_ARG_ENABLE([alignment-checks],
  AS_HELP_STRING([--enable-alignment-checks],
                 [Enable runtime checks for cast alignment @<:@default=no@:>@]),
  [],
  enable_alignment_checks="no")
AS_IF([test "x$enable_alignment_checks" = "xyes"],
      [AC_DEFINE([SPICE_DEBUG_ALIGNMENT], 1, [Enable runtime checks for cast alignment])])

SPICE_CHECK_INSTRUMENTATION

# Checks for libraries
PKG_CHECK_MODULES([PROTOCOL], [spice-protocol >= 0.12.12])

SPICE_CHECK_PYTHON_MODULES()

SPICE_CHECK_PIXMAN
SPICE_CHECK_SMARTCARD
SPICE_CHECK_CELT051
SPICE_CHECK_GLIB2
SPICE_CHECK_OPUS
SPICE_CHECK_OPENSSL
SPICE_CHECK_GDK_PIXBUF

SPICE_COMMON_CFLAGS='$(PIXMAN_CFLAGS) $(SMARTCARD_CFLAGS) $(CELT051_CFLAGS) $(GLIB2_CFLAGS) $(OPUS_CFLAGS) $(OPENSSL_CFLAGS)'
SPICE_COMMON_CFLAGS="$SPICE_COMMON_CFLAGS -DG_LOG_DOMAIN=\\\"Spice\\\""
SPICE_COMMON_LIBS='$(PIXMAN_LIBS) $(CELT051_LIBS) $(GLIB2_LIBS) $(OPUS_LIBS) $(OPENSSL_LIBS)'
AC_SUBST(SPICE_COMMON_CFLAGS)
AC_SUBST(SPICE_COMMON_LIBS)

# The End!
AC_CONFIG_FILES([
  Makefile
  common/Makefile
  python_modules/Makefile
  tests/Makefile
  docs/Makefile
])

AC_OUTPUT
