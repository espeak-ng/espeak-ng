AC_PREREQ([2.63])
AC_INIT([eSpeak NG], [1.48.15], [https://github.com/espeak-ng/espeak-ng/issues], [espeak-ng], [https://github.com/espeak-ng/espeak-ng])
AM_INIT_AUTOMAKE()
LT_INIT

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_LN_S

dnl ================================================================
dnl C99 compiler checks.
dnl ================================================================

AC_LANG_PUSH(C)
AC_MSG_CHECKING([if $CC supports C99 without any flags])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[[static inline int c99_restrict(int *restrict c99_restrict) {}]],
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
		[[static inline int c99_restrict(int *restrict c99_restrict) {}]],
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

dnl ================================================================
dnl library checks.
dnl ================================================================

AC_CHECK_HEADERS([fcntl.h])    dnl POSIX
AC_CHECK_HEADERS([getopt.h])   dnl POSIX
AC_CHECK_HEADERS([locale.h])   dnl C89
AC_CHECK_HEADERS([stddef.h])   dnl C89
AC_CHECK_HEADERS([stdbool.h])  dnl C99
AC_CHECK_HEADERS([sys/time.h]) dnl POSIX
AC_CHECK_HEADERS([wchar.h])    dnl C89
AC_CHECK_HEADERS([wctype.h])   dnl C89

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_FUNC_ERROR_AT_LINE

AC_CHECK_FUNCS([dup2])
AC_CHECK_FUNCS([getopt_long])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memchr])
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strstr])

dnl ================================================================
dnl PulseAudio checks.
dnl ================================================================

AC_ARG_WITH([pulseaudio],
    [AS_HELP_STRING([--with-pulseaudio], [use the pulseaudio library for audio output @<:@default=yes@:>@])],
    [])

if test "$with_pulseaudio" = "no"; then
    echo "Disabling pulseaudio output support via pulseaudio"
    have_pulseaudio=no
else
    PKG_CHECK_MODULES(PULSEAUDIO, [libpulse >= 0.9],
        [
            have_pulseaudio=yes
        ],[
            have_pulseaudio=no
        ])
fi

dnl ================================================================
dnl PortAudio checks.
dnl ================================================================

AC_ARG_WITH([portaudio],
    [AS_HELP_STRING([--with-portaudio], [use the portaudio library for audio output @<:@default=yes@:>@])],
    [])

if test "$with_portaudio" = "no"; then
    echo "Disabling portaudio output support via portaudio"
    have_portaudio=no
else
    AC_CHECK_HEADERS([portaudio.h],
        [
            # Check the portaudio library.
            AC_CHECK_LIB([portaudio], [Pa_IsStreamActive]) # portaudio 19
            AC_CHECK_LIB([portaudio], [Pa_StreamActive])   # portaudio 18

            # Then use the headers to determine the portaudio version.
            # This is because on some systems with both libportaudio0 and
            # libportaudio2 installed, portaudio.h and -lportaudio refer
            # to different versions.
            AC_CHECK_FUNC([Pa_IsStreamActive],
                [
                    have_portaudio=19
                ],[
                    AC_CHECK_FUNC([Pa_StreamActive],
                        [
                            have_portaudio=18
                        ],[
                            have_portaudio=no
                        ])
                ])
        ],[
            have_portaudio=no
        ])
fi

dnl ================================================================
dnl Audio checks.
dnl ================================================================

AC_ARG_WITH([sada],
    [AS_HELP_STRING([--with-sada], [use the Solaris SADA audio API @<:@default=no@:>@])],
    [])

if test "$with_sada" = "yes" ; then
	have_sada=yes
else
	have_sada=no
fi

if test "$have_portaudio" = 18 -o "$have_portaudio" = 19 ; then
    if test "$have_pulseaudio" = yes ; then
        PKG_CHECK_MODULES(PULSEAUDIO_SIMPLE, [libpulse-simple >= 0.9],
            [
                have_pulseaudio=yes
                AUDIO=runtime
            ],[
                have_pulseaudio=no
                AUDIO=portaudio
            ])
    else
        AUDIO=portaudio
    fi
elif test "$have_pulseaudio" = yes ; then
    AUDIO=pulseaudio
elif test "$have_sada" = yes ; then
    AUDIO=sada
else
    AUDIO=disabled
fi

AC_SUBST(AUDIO)

AM_CONDITIONAL(AUDIO_RUNTIME,    [test x"$AUDIO" = xruntime])
AM_CONDITIONAL(AUDIO_PULSEAUDIO, [test x"$AUDIO" = xpulseaudio])
AM_CONDITIONAL(AUDIO_PORTAUDIO,  [test x"$AUDIO" = xportaudio])
AM_CONDITIONAL(AUDIO_SADA,       [test x"$AUDIO" = xsada])

dnl ================================================================
dnl Optional compilation checks.
dnl ================================================================

AC_ARG_WITH([klatt],
    [AS_HELP_STRING([--with-klatt], [enable the Klatt formant synthesizer @<:@default=yes@:>@])],
    [])

if test "$with_klatt" = "no" ; then
	have_klatt=no
else
	have_klatt=yes
fi

AC_ARG_WITH([mbrola],
    [AS_HELP_STRING([--with-mbrola], [enable the MBROLA speech synthesizer @<:@default=yes@:>@])],
    [])

if test "$with_mbrola" = "no" ; then
	have_mbrola=no
else
	have_mbrola=yes
fi

AC_ARG_WITH([sonic],
    [AS_HELP_STRING([--with-sonic], [enable the sonic library to speed up the audio @<:@default=yes@:>@])],
    [])

if test "$with_sonic" = "no" ; then
    have_sonic=no
else
    AC_CHECK_HEADERS([sonic.h],
        [
            AC_CHECK_LIB([sonic], [sonicCreateStream])
            have_sonic=yes
        ],[
            have_sonic=no
        ])
fi

AC_ARG_WITH([async],
    [AS_HELP_STRING([--with-async], [enable support for async command processing @<:@default=yes@:>@])],
    [])

if test "$with_async" = "no" ; then
	have_async=no
else
	have_async=yes
fi

AM_CONDITIONAL(OPT_KLATT,  [test x"$have_klatt"  = xyes])
AM_CONDITIONAL(OPT_MBROLA, [test x"$have_mbrola" = xyes])
AM_CONDITIONAL(OPT_ASYNC,  [test x"$have_async"  = xyes])

dnl ================================================================
dnl ronn checks.
dnl ================================================================

AC_CHECK_PROG(RONN, ronn, ronn, no)

AM_CONDITIONAL(HAVE_RONN, [test ! x"$RONN" = xno])

dnl ================================================================
dnl Extended dictionary checks.
dnl ================================================================

AC_ARG_WITH([extdict-ru],
    [AS_HELP_STRING([--with-extdict-ru], [use the extended Russian Dictionary file @<:@default=no@:>@])],
    [])

if test "$with_extdict_ru" = "yes" ; then
	have_extdict_ru=yes
else
	have_extdict_ru=no
fi

AC_ARG_WITH([extdict-zh],
    [AS_HELP_STRING([--with-extdict-zh], [use the extended Mandarin Chinese Dictionary file @<:@default=no@:>@])],
    [])

if test "$with_extdict_zh" = "yes" ; then
	have_extdict_zh=yes
else
	have_extdict_zh=no
fi

AC_ARG_WITH([extdict-zhy],
    [AS_HELP_STRING([--with-extdict-zhy], [use the extended Cantonese Chinese Dictionary file @<:@default=no@:>@])],
    [])

if test "$with_extdict_zhy" = "yes" ; then
	have_extdict_zhy=yes
else
	have_extdict_zhy=no
fi

AM_CONDITIONAL(HAVE_RU_EXTENDED_DICTIONARY,  [test x"$have_extdict_ru"  = xyes])
AM_CONDITIONAL(HAVE_ZH_EXTENDED_DICTIONARY,  [test x"$have_extdict_zh"  = xyes])
AM_CONDITIONAL(HAVE_ZHY_EXTENDED_DICTIONARY, [test x"$have_extdict_zhy" = xyes])

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for eSpeak NG complete.

        Source code location:          ${srcdir}

        C99 Compiler:                  ${CC}
        C99 Compiler flags:            ${CFLAGS}

        pulseaudio:                    ${have_pulseaudio}
        portaudio:                     ${have_portaudio}
        sada:                          ${have_sada}
        audio configuration:           ${AUDIO}

        Klatt:                         ${have_klatt}
        MBROLA:                        ${have_mbrola}
        Sonic:                         ${have_sonic}
        Async:                         ${have_async}

        Extended Dictionaries:
            Russian:                   ${have_extdict_ru}
            Chinese (Mandarin):        ${have_extdict_zh}
            Chinese (Cantonese):       ${have_extdict_zhy}
])
