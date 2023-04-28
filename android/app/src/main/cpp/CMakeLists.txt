cmake_minimum_required(VERSION 3.8)

project(espeak-android)

add_subdirectory(../../../../../ espeakng)
add_custom_target(
        espeak-data
        COMMAND ${CMAKE_COMMAND} -B${CMAKE_CURRENT_BINARY_DIR}/../espeak-data ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/../espeak-data --target data
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/../espeak-data/espeak-ng-data ${CMAKE_SOURCE_DIR}/../../../build/generated/espeak-ng-data
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
