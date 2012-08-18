AC_PREREQ([2.65])
AC_INIT([eSpeak], [1.46], [https://github.com/rhdunn/espeak/issues], [espeak], [https://github.com/rhdunn/espeak])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl getopt checks.
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for eSpeak complete.

        Source code location:          ${srcdir}
        XDG data location:             ${XDGDATADIR}

        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}
])
