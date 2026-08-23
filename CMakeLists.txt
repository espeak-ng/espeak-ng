cmake_minimum_required(VERSION 3.8...3.10)

project(espeak-ng
  VERSION 1.53.0
  DESCRIPTION "open source speech synthesizer that supports more than hundred languages and accents"
  HOMEPAGE_URL "https://github.com/espeak-ng/espeak-ng"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(COMPILE_INTONATIONS "Intonation compilation" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(WITH_COVERAGE "Enable clang source-based coverage instrumentation" OFF)
option(WITH_FUZZER "Build libFuzzer targets (requires Clang)" OFF)
option(WITH_ASAN "Enable ASAN" OFF)
option(WITH_LSAN "Enable LSAN" OFF)

if(APPLE)
  # Required to expose standard POSIX/BSD APIs needed for compilation
  add_compile_definitions(_DARWIN_C_SOURCE)
endif()

include(CTest)

if (WITH_COVERAGE OR WITH_FUZZER)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "^(Apple)?Clang$")
    message(FATAL_ERROR "WITH_COVERAGE and WITH_FUZZER require Clang")
  endif()
endif()

if (WITH_COVERAGE)
  add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()

if (WITH_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
  if (CMAKE_C_COMPILER_ID MATCHES "AppleClang")
    find_program(ASAN_SYMBOLIZER_PATH NAMES atos)
    list(APPEND ESPEAK_RUN_ENV "ASAN_SYMBOLIZER_PATH=${ASAN_SYMBOLIZER_PATH}")
  elseif (CMAKE_C_COMPILER_ID MATCHES "Clang")
    find_program(ASAN_SYMBOLIZER_PATH NAMES llvm-symbolizer)
    list(APPEND ESPEAK_RUN_ENV "ASAN_SYMBOLIZER_PATH=${ASAN_SYMBOLIZER_PATH}")
  endif()
  list(APPEND ESPEAK_RUN_ENV MallocNanoZone=0 ASAN_OPTIONS=detect_leaks=0:symbolize=1)
endif()

if (WITH_LSAN)
  add_compile_options(-fsanitize=leak -fno-omit-frame-pointer)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=leak")
  list(APPEND ESPEAK_RUN_ENV LSAN_OPTIONS=fast_unwind_on_malloc=0)
endif()

include(GNUInstallDirs)
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
