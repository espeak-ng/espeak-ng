cmake_minimum_required(VERSION 3.8)

project(espeak-ng
  VERSION 1.52.0.1
  DESCRIPTION "eSpeak NG"
  HOMEPAGE_URL "https://github.com/espeak-ng/espeak-ng"
)

include(CTest)

include(cmake/deps.cmake)
include(cmake/config.cmake)
add_subdirectory(src)
include(cmake/data.cmake)

include(cmake/package.cmake)
include(CPack)

include(CTest)
add_subdirectory(tests)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

message(STATUS "Configuration:")
message(STATUS "  shared: ${BUILD_SHARED_LIBS}")
message(STATUS "  mbrola: ${USE_MBROLA} (${MBROLA_BIN})")
message(STATUS "  libsonic: ${USE_LIBSONIC} (${SONIC_LIB} ${SONIC_INC})")
message(STATUS "  libpcaudio: ${USE_LIBPCAUDIO} (${PCAUDIO_LIB} ${PCAUDIO_INC})")
message(STATUS "  klatt: ${USE_KLATT}")
message(STATUS "  speech-player: ${USE_SPEECHPLAYER}")
message(STATUS "  async: ${USE_ASYNC}")
