AC_PREREQ([2.63])
AC_INIT([eSpeak], [1.48], [https://github.com/rhdunn/espeak/issues], [espeak], [https://github.com/rhdunn/espeak])
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

AC_CHECK_PROG(NDKBUILD_CHECK,[ndk-build],yes)
if test x"$NDKBUILD_CHECK" != xyes ; then
    AC_MSG_ERROR([Please ensure that the Android NDK is installed and ndk-build is usable from the command line.])
fi

dnl ================================================================
dnl gradle checks.
dnl ================================================================

AC_ARG_WITH([gradle],
    [AS_HELP_STRING([--with-gradle], [specify the gradle program to use @<:@default=gradle@:>@])],
    [GRADLE=$with_gradle],
    [GRADLE=gradle])

AC_MSG_CHECKING([for gradle])
if test -e ${GRADLE} ; then
    AC_MSG_RESULT([${GRADLE}])
else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([The gradle command '${GRADLE}' is not found.])
fi

AC_SUBST(GRADLE)

dnl ================================================================
dnl getopt checks.
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

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
	have_sonic=yes
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
AM_CONDITIONAL(OPT_SONIC,  [test x"$have_sonic"  = xyes])
AM_CONDITIONAL(OPT_ASYNC,  [test x"$have_async"  = xyes])

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
dnl wxWidgets checks.
dnl ================================================================

AC_ARG_WITH([wx-config],
    [AS_HELP_STRING([--with-wx-config], [specify the location of wx-config @<:@default=wx-config@:>@])],
    [WX_CONFIG=$with_wx_config],
    [WX_CONFIG=wx-config])

if test ! -e "${WX_CONFIG}" ; then
    AC_CHECK_PROG(WXCONFIG_CHECK,${WX_CONFIG},yes)
    if test x"$WXCONFIG_CHECK" != x"yes" ; then
        AC_MSG_ERROR([Cannot find wxWidgets which is needed for espeakedit to build the voices.])
    fi
fi

WX_LIBS=`${WX_CONFIG} --libs`
WX_CXXFLAGS=`${WX_CONFIG} --cxxflags`
WX_VERSION=`${WX_CONFIG} --version`

AC_SUBST(WX_LIBS)
AC_SUBST(WX_CXXFLAGS)

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for eSpeak complete.

        Source code location:          ${srcdir}

        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}

        wxWidgets:                     ${WX_VERSION} (${WX_CONFIG})
        pulseaudio:                    ${have_pulseaudio}
        portaudio:                     ${have_portaudio}
        sada:                          ${have_sada}
        audio configuration:           ${AUDIO}

        gradle:                        ${GRADLE}

        Klatt:                         ${have_klatt}
        MBROLA:                        ${have_mbrola}
        Sonic:                         ${have_sonic}
        Async:                         ${have_async}

        Extended Dictionaries:
            Russian:                   ${have_extdict_ru}
            Chinese (Mandarin):        ${have_extdict_zh}
            Chinese (Cantonese):       ${have_extdict_zhy}
])
