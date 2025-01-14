AC_PREREQ([2.63])
AC_INIT([eSpeak NG], [1.52.0], [https://github.com/espeak-ng/espeak-ng/issues], [espeak-ng], [https://github.com/espeak-ng/espeak-ng])
AM_INIT_AUTOMAKE()
LT_INIT

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl ================================================================
dnl configure arguments
dnl ================================================================

AC_ARG_ENABLE([rpath],
    [AS_HELP_STRING([--enable-rpath], [set hardcoded rpaths in the executable @<:@default=yes@:>@])],
    [],
    [enable_rpath=yes])

AC_ARG_WITH([gradle],
    [AS_HELP_STRING([--with-gradle], [specify the gradle program to use @<:@default=gradle@:>@])],
    [GRADLE=$with_gradle],
    [GRADLE=gradle])

AC_ARG_WITH([pcaudiolib],
    [AS_HELP_STRING([--with-pcaudiolib], [use the pcaudiolib library for audio output @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([klatt],
    [AS_HELP_STRING([--with-klatt], [enable the Klatt formant synthesizer @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([speechplayer],
    [AS_HELP_STRING([--with-speechplayer], [enable the speechPlayer Klatt implementation @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([mbrola],
    [AS_HELP_STRING([--with-mbrola], [enable the MBROLA speech synthesizer @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([sonic],
    [AS_HELP_STRING([--with-sonic], [enable the sonic library to speed up the audio @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([async],
    [AS_HELP_STRING([--with-async], [enable support for async command processing @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([extdict-ru],
    [AS_HELP_STRING([--with-extdict-ru], [use the extended Russian Dictionary file @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([extdict-cmn],
    [AS_HELP_STRING([--with-extdict-cmn], [use the extended Mandarin Chinese Dictionary file @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([extdict-yue],
    [AS_HELP_STRING([--with-extdict-yue], [use the extended Cantonese Chinese Dictionary file @<:@default=yes@:>@])],
    [])

AC_ARG_WITH([libfuzzer],
    [AS_HELP_STRING([--with-libfuzzer], [enable libFuzzer in the fuzzer tests @<:@default=no@:>@])],
    [])

AC_ARG_WITH([coverage],
    [AS_HELP_STRING([--with-coverage], [enable clang coverage in the fuzzer tests (also add coverage to lib sources) @<:@default=no@:>@])],
    [])

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_LN_S

dnl ================================================================
dnl gradle checks.
dnl ================================================================

AC_MSG_CHECKING([for gradle])
if command -v "$GRADLE" >/dev/null; then
    AC_MSG_RESULT([$GRADLE])
else
    GRADLE=
    AC_MSG_RESULT([no])
fi
AM_CONDITIONAL(HAVE_GRADLE, [test x"$GRADLE" != x])

AC_SUBST(GRADLE)

dnl ================================================================
dnl C99 compiler checks.
dnl ================================================================

AC_LANG_PUSH(C)
AC_MSG_CHECKING([if $CC supports C99 without any flags])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[[static inline void c99_restrict(int *restrict c99_restrict) {}]],
		[[]])],
	[AC_MSG_RESULT([yes])
	 have_c99_native=yes],
	[AC_MSG_RESULT([no])
	 have_c99_native=no])
AC_LANG_POP(C)

AC_LANG_PUSH(C)
TEMP_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -std=c99"
AC_MSG_CHECKING([if $CC supports C99 with the -std=c99 flag])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[[static inline void c99_restrict(int *restrict c99_restrict) {}]],
		[[]])],
	[AC_MSG_RESULT([yes])
	 have_c99_c99=yes],
	[AC_MSG_RESULT([no])
	 have_c99_c99=no])
CFLAGS="$TEMP_CFLAGS"
AC_LANG_POP(C)

AC_MSG_CHECKING([if $CC supports C99])
if test "$have_c99_c99" = yes ; then
	AC_MSG_RESULT([-std=c99])
	CFLAGS="$CFLAGS -std=c99"
elif test "$have_c99_native" = yes ; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([C99 is not supported by $CC.])
fi


if test "$with_libfuzzer" = yes ; then
	AC_LANG_PUSH(C)
	TEMP_CFLAGS="$CFLAGS"
	TEMP_LIBS="$LIBS"
	HOST_CPU="`$CC -print-target-triple | sed -e 's/-.*//'`"
	LIBFUZZER_NO_MAIN="`$CC -print-file-name=libclang_rt.fuzzer_no_main-$HOST_CPU.a` -lstdc++ -lm"
	LIBFUZZER="`$CC -print-file-name=libclang_rt.fuzzer-$HOST_CPU.a` -lstdc++ -lm"
	CFLAGS="$CFLAGS -fsanitize=fuzzer-no-link"
	LIBS="$LIBS $LIBFUZZER_NO_MAIN"
	AC_MSG_CHECKING([if $CC supports fuzzer with the -fsanitize=fuzzer flag])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM( [[]], [[]])],
		[have_fuzzer_fuzzer=yes],
		[have_fuzzer_fuzzer=no])
	AC_MSG_RESULT($have_fuzzer_fuzzer)
	if test "$have_fuzzer_fuzzer" != yes ; then
		CFLAGS="$TEMP_CFLAGS"
		LIBFUZZER_NO_MAIN=""
		LIBFUZZER=""
	fi
	AC_SUBST(LIBFUZZER_NO_MAIN)
	AC_SUBST(LIBFUZZER)
	LIBS="$TEMP_LIBS"
	AC_LANG_POP(C)
fi

dnl ================================================================
dnl FreeBSD check.
dnl ================================================================

AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
		   #if ! defined(__FreeBSD__)
		   #error macro not defined
		   #endif
		   ]])], [FREEBSD_FOUND="yes"], [FREEBSD_FOUND="no"])

AC_MSG_CHECKING([if targeting FreeBSD])
if test "$FREEBSD_FOUND" = "yes" ; then
	AC_MSG_RESULT([-D__BSD_VISIBLE])
	CFLAGS="$CFLAGS -D__BSD_VISIBLE"
else
	AC_MSG_RESULT([no])
fi

dnl ================================================================
dnl library checks.
dnl ================================================================

AC_CHECK_HEADERS([endian.h])     dnl Linux
AC_CHECK_HEADERS([fcntl.h])      dnl POSIX
AC_CHECK_HEADERS([getopt.h])     dnl POSIX
AC_CHECK_HEADERS([locale.h])     dnl C89
AC_CHECK_HEADERS([stddef.h])     dnl C89
AC_CHECK_HEADERS([stdbool.h])    dnl C99
AC_CHECK_HEADERS([sys/endian.h]) dnl BSD
AC_CHECK_HEADERS([sys/time.h])   dnl POSIX
AC_CHECK_HEADERS([wchar.h])      dnl C89
AC_CHECK_HEADERS([wctype.h])     dnl C89

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_FUNC_FORK
AC_FUNC_STRCOLL
AC_FUNC_ERROR_AT_LINE

AC_CHECK_FUNCS([dup2])
AC_CHECK_FUNCS([getopt_long])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([malloc]) dnl Avoid "Undefined reference to rpl_malloc" when using AC_FUNC_MALLOC.
AC_CHECK_FUNCS([memchr])
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([mkstemp])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([realloc]) dnl Avoid "Undefined reference to rpl_malloc" when using AC_FUNC_REALLOC.
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strstr])

dnl ================================================================
dnl PCAudioLib checks.
dnl ================================================================

if test "$with_pcaudiolib" = "no"; then
    echo "Disabling audio output support via pcaudiolib"
    have_pcaudiolib=no
else
    AC_CHECK_HEADERS([pcaudiolib/audio.h],
        [
            have_pcaudiolib=yes
            PCAUDIOLIB_CFLAGS=-DUSE_LIBPCAUDIO=1
            PCAUDIOLIB_LIBS=-lpcaudio
        ],[
            have_pcaudiolib=no
        ])
fi

AC_SUBST(PCAUDIOLIB_CFLAGS)
AC_SUBST(PCAUDIOLIB_LIBS)

dnl ================================================================
dnl Optional compilation checks.
dnl ================================================================

if test "$with_klatt" = "no" ; then
	have_klatt=no
else
	have_klatt=yes
fi

if test "$with_speechplayer" = "no" ; then
	have_speechplayer=no
else
	have_speechplayer=yes
fi

if test "$with_mbrola" = "no" ; then
	have_mbrola=no
else
	have_mbrola=yes
fi

if test "$with_sonic" = "no" ; then
    have_sonic=no
else
    AC_CHECK_HEADERS([sonic.h],
        [
            SONIC_CFLAGS=-DUSE_LIBSONIC=1
            AC_CHECK_LIB([sonic], [sonicCreateStream])
            have_sonic=yes
        ],[
            have_sonic=no
        ])
fi
AC_SUBST(SONIC_CFLAGS)

AC_CHECK_HEADERS([valgrind/memcheck.h])

if test "$with_async" = "no" ; then
	have_async=no
else
	have_async=yes
fi

AM_CONDITIONAL(OPT_KLATT,  [test x"$have_klatt"  = xyes])
AM_CONDITIONAL(OPT_SPEECHPLAYER,  [test x"$have_speechplayer"  = xyes])
AM_CONDITIONAL(OPT_MBROLA, [test x"$have_mbrola" = xyes])
AM_CONDITIONAL(OPT_ASYNC,  [test x"$have_async"  = xyes])

dnl ================================================================
dnl ronn checks.
dnl ================================================================

AC_CHECK_PROG(RONN, ronn, ronn, no)

AM_CONDITIONAL(HAVE_RONN, [test ! x"$RONN" = xno])

AC_CHECK_PROG(KRAMDOWN, kramdown, kramdown, no)

dnl ================================================================
dnl Extended dictionary checks.
dnl ================================================================

if test "$with_extdict_ru" = "no" ; then
	have_extdict_ru=no
else
	have_extdict_ru=yes
fi

if test "$with_extdict_cmn" = "no" ; then
	have_extdict_cmn=no
else
	have_extdict_cmn=yes
fi

if test "$with_extdict_yue" = "no" ; then
	have_extdict_yue=no
else
	have_extdict_yue=yes
fi

AM_CONDITIONAL(HAVE_RU_EXTENDED_DICTIONARY,  [test x"$have_extdict_ru"  = xyes])
AM_CONDITIONAL(HAVE_CMN_EXTENDED_DICTIONARY,  [test x"$have_extdict_cmn"  = xyes])
AM_CONDITIONAL(HAVE_YUE_EXTENDED_DICTIONARY, [test x"$have_extdict_yue" = xyes])

dnl ================================================================
dnl Compiler warnings.
dnl
dnl Reference: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
dnl ================================================================

AX_CHECK_COMPILE_FLAG([-Wimplicit],              [CFLAGS="-Wimplicit $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wint-conversion],        [CFLAGS="-Wint-conversion $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes],    [CFLAGS="-Wmissing-prototypes $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wreturn-type],           [CFLAGS="-Wreturn-type $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wuninitialized],         [CFLAGS="-Wuninitialized $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wunused],                [CFLAGS="-Wunused $CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wunused-parameter],      [CFLAGS="-Wunused-parameter $CFLAGS"])


dnl ================================================================
dnl Enable two-complement signed integer wrapping
dnl
dnl Otherwise this is undefined behavior.
dnl ================================================================

AX_CHECK_COMPILE_FLAG([-fwrapv],                 [CFLAGS="-fwrapv $CFLAGS"])

dnl ================================================================
dnl libFuzzer checks.
dnl ================================================================

if test "$with_libfuzzer" = "yes" ; then
	have_libfuzzer=yes
else
	have_libfuzzer=no
fi

AM_CONDITIONAL(HAVE_LIBFUZZER, [test x"$have_libfuzzer" = xyes  -a x"$have_fuzzer_fuzzer" = xyes])

dnl ================================================================
dnl clang-coverage checks.
dnl ================================================================

if test "$with_coverage" = "yes" ; then
	use_coverage=yes
else
	use_coverage=no
fi

AM_CONDITIONAL(USE_COVERAGE, [test x"$use_coverage" = xyes])

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile espeak-ng.pc])
AC_OUTPUT

if test "$enable_rpath" = "yes" ; then
	true
else
	# See https://old-en.opensuse.org/openSUSE:Packaging_Guidelines#Removing_Rpath
	sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
	sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
fi

AC_MSG_NOTICE([

    Configuration for eSpeak NG complete.

        Source code location:          ${srcdir}

        C99 Compiler:                  ${CC}
        C99 Compiler flags:            ${CFLAGS}

        Sonic:                         ${have_sonic}
        PCAudioLib:                    ${have_pcaudiolib}

        gradle (Android):              ${GRADLE}

        Klatt:                         ${have_klatt}
        speechPlayer:                  ${have_speechplayer}
        MBROLA:                        ${have_mbrola}
        Async:                         ${have_async}

        Extended Dictionaries:
            Russian:                   ${have_extdict_ru}
            Chinese (Mandarin):        ${have_extdict_cmn}
            Chinese (Cantonese):       ${have_extdict_yue}
])
