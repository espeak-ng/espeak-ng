include(CheckSymbolExists)
include(CheckIncludeFile)

check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
check_include_file("nbtool_config.h" HAVE_NBTOOL_CONFIG_H)
check_symbol_exists(optreset "getopt.h;unistd.h" HAVE_DECL_OPTRESET)
check_include_file("sys/endian.h" HAVE_SYS_ENDIAN_H)
check_symbol_exists(iswblank "wctype.h" HAVE_ISWBLANK)

option(USE_MBROLA "Use mbrola for speech synthesis" ${HAVE_MBROLA})
option(USE_LIBSONIC "Use libsonic for faster speech rates" ${HAVE_LIBSONIC})
option(USE_LIBPCAUDIO "Use libPcAudio for sound output" ${HAVE_LIBPCAUDIO})

option(USE_KLATT "Use klatt for speech synthesis" ON)
option(USE_SPEECHPLAYER "Use speech-player for speech synthesis" ON)
if (HAVE_PTHREAD)
  option(USE_ASYNC "Support asynchronous speech synthesis" ON)
else()
  set(USE_ASYNC OFF)
endif()

option(ESPEAK_COMPAT "Install compat binary symlinks" ON)
