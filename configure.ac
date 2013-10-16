AC_PREREQ([2.65])
AC_INIT([Unicode Character Database Tools], [0.1], [https://github.com/rhdunn/ucd-tools/issues], [ucd-tools], [https://github.com/rhdunn/ucd-tools])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

LT_INIT

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl UCD data configuration.
dnl ================================================================

AC_ARG_WITH([unicode-version],
    [AS_HELP_STRING([--with-unicode-version], [Unicode version to support @<:@default=6.2.0@:>@])],
    [AS_IF([test x"$withval" != x],
           [UCD_VERSION="$withval"])],
    [UCD_VERSION="6.2.0"])

AC_SUBST(UCD_VERSION)

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for Unicode Character Data Tools complete.

        Source code location:          ${srcdir}

        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}

        Unicode version:               ${UCD_VERSION}
])
