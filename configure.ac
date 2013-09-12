AC_PREREQ([2.63])
AC_INIT([eSpeak], [1.47], [https://github.com/rhdunn/espeak/issues], [espeak], [https://github.com/rhdunn/espeak])
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
else
    AUDIO=disabled
fi

AC_SUBST(AUDIO)

AM_CONDITIONAL(AUDIO_RUNTIME,    [test x"$AUDIO" = xruntime])
AM_CONDITIONAL(AUDIO_PULSEAUDIO, [test x"$AUDIO" = xpulseaudio])
AM_CONDITIONAL(AUDIO_PORTAUDIO,  [test x"$AUDIO" = xportaudio])
AM_CONDITIONAL(AUDIO_SADA,       [test x"$AUDIO" = xsada])

dnl ================================================================
dnl wxWidgets checks.
dnl ================================================================

AC_CHECK_PROG(WXCONFIG_CHECK,wx-config,yes)
if test x"$WXCONFIG_CHECK" != x"yes" ; then
    AC_MSG_ERROR([Please install wxWidgets which is needed for espeakedit.])
fi

WX_LIBS=`wx-config --libs`
WX_CXXFLAGS=`wx-config --cxxflags`
WX_VERSION=`wx-config --version`

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

        wxWidgets:                     ${WX_VERSION}
	pulseaudio:                    ${have_pulseaudio}
	portaudio:                     ${have_portaudio}
	audio configuration:           ${AUDIO}
])
