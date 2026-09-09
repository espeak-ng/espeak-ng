include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
check_include_file("nbtool_config.h" HAVE_NBTOOL_CONFIG_H)
check_symbol_exists(optreset "getopt.h;unistd.h" HAVE_DECL_OPTRESET)
check_include_file("sys/endian.h" HAVE_SYS_ENDIAN_H)
check_symbol_exists(iswblank "wctype.h" HAVE_ISWBLANK)

# The WAV tests compare an exact sha1 of the synthesized audio against hashes
# that were generated on targets which evaluate float and double at the
# precision of the type. A target that keeps excess intermediate precision --
# in practice the x87 FPU without SSE math, where FLT_EVAL_METHOD is 2 --
# rounds differently, so every one of those hashes differs. See #1270.
check_c_source_compiles("
#if !defined(__FLT_EVAL_METHOD__) || __FLT_EVAL_METHOD__ != 2
#error floating point is evaluated at the precision of the type
#endif
int main(void) { return 0; }
" HAVE_EXCESS_FP_PRECISION)

if (HAVE_EXCESS_FP_PRECISION)
  set(ESPEAK_EXCESS_FP_PRECISION 1)
else()
  set(ESPEAK_EXCESS_FP_PRECISION 0)
endif()

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
