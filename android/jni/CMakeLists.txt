cmake_minimum_required(VERSION 3.14)

project(espeak-android)

# Build libsonic from source for Android and pre-seed the find results so
# cmake/deps.cmake in the main project detects it (the NDK sysroot has no sonic).
include(FetchContent)
FetchContent_Declare(sonic-git
        GIT_REPOSITORY https://github.com/waywardgeek/sonic.git
        GIT_TAG fbf75c3d6d846bad3bb3d456cbc5d07d9fd8c104
)
FetchContent_MakeAvailable(sonic-git)
add_library(sonic OBJECT ${sonic-git_SOURCE_DIR}/sonic.c)
# enable PIC in order to embed libsonic in libttsespeak.so
set_property(TARGET sonic PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(sonic PUBLIC ${sonic-git_SOURCE_DIR})
set(SONIC_LIB sonic)
set(SONIC_INC ${sonic-git_SOURCE_DIR})

add_subdirectory(../../ espeakng)

add_custom_target(
        espeak-data
        COMMAND ${CMAKE_COMMAND} -B${CMAKE_CURRENT_BINARY_DIR}/../espeak-data ${CMAKE_CURRENT_SOURCE_DIR}/../../
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/../espeak-data --target data
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/../espeak-data/espeak-ng-data ${CMAKE_SOURCE_DIR}/../build/generated/espeak-ng-data
)

message(STATUS "Data: ${CMAKE_CURRENT_SOURCE_DIR}")

find_library(A_LOG log)

add_library(ttsespeak SHARED
        jni/eSpeakService.c
        )
target_include_directories(ttsespeak PRIVATE
        include
)
target_link_libraries(ttsespeak PRIVATE
        espeak-ng ${A_LOG}
)
