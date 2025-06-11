cmake_minimum_required(VERSION 3.8)

project(espeak-ng
  VERSION 1.52.0.1
  DESCRIPTION "open source speech synthesizer that supports more than hundred languages and accents"
  HOMEPAGE_URL "https://github.com/espeak-ng/espeak-ng"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(COMPILE_INTONATIONS "Intonation compilation" ON)
option(ENABLE_TESTS "Enable tests" ON)

include(CTest)

include(cmake/deps.cmake)
include(cmake/config.cmake)
add_subdirectory(src)

if (COMPILE_INTONATIONS)
  if (CMAKE_CROSSCOMPILING)
    find_program(NATIVEBUILD NAMES espeak-ng PATHS ${NativeBuild_DIR} NO_DEFAULT_PATH)
    if(NATIVEBUILD)
      message(STATUS "Native build of espeak-ng is located at: ${NATIVEBUILD}")
      include(cmake/data.cmake)
    else()
      message(STATUS "Not building intonations as the build is a cross compile.")
      message(STATUS "Call CMake with the location of the native build of espeak-ng")
      message(STATUS "  e.g. -DNativeBuild=~/src/build-native")
      message(STATUS "to build the intonations.")
    endif()
  else()
    include(cmake/data.cmake)
  endif()
endif()
include(cmake/docs.cmake)

include(cmake/package.cmake)
include(CPack)

if(ENABLE_TESTS AND NOT CMAKE_CROSSCOMPILING)
  add_subdirectory(tests)
endif()

message(STATUS "Configuration:")
message(STATUS "  shared: ${BUILD_SHARED_LIBS}")
message(STATUS "  mbrola: ${USE_MBROLA} (${MBROLA_BIN})")
message(STATUS "  libsonic: ${USE_LIBSONIC} (${SONIC_LIB} ${SONIC_INC})")
message(STATUS "  libpcaudio: ${USE_LIBPCAUDIO} (${PCAUDIO_LIB} ${PCAUDIO_INC})")
message(STATUS "  klatt: ${USE_KLATT}")
message(STATUS "  speech-player: ${USE_SPEECHPLAYER}")
message(STATUS "  async: ${USE_ASYNC}")

install(
  DIRECTORY vim/ftdetect vim/syntax DESTINATION share/vim/vimfiles
)
