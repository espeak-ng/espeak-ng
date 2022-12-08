add_library(espeak-include INTERFACE)
target_include_directories(espeak-include INTERFACE include include/compat)

add_subdirectory(ucd-tools)
add_subdirectory(speechPlayer)
add_subdirectory(libespeak-ng)

add_executable(espeak-ng-bin espeak-ng.c)
set_target_properties(espeak-ng-bin PROPERTIES OUTPUT_NAME espeak-ng)
target_link_libraries(
  espeak-ng-bin PRIVATE espeak-ng espeak-ng-config
)
if (MINGW)
  target_link_options(espeak-ng-bin PRIVATE "-static-libstdc++" "-static")
endif()
if (MSVC)
  target_sources(espeak-ng-bin PRIVATE compat/getopt.c)
endif()
if (NOT WIN32)
  add_custom_target(
    speak-ng ALL
    ${CMAKE_COMMAND} -E create_symlink espeak-ng ${CMAKE_CURRENT_BINARY_DIR}/speak-ng
    COMMENT "Link espeak-ng to speak-ng"
    DEPENDS espeak-ng-bin
  )
endif()
install(TARGETS espeak-ng-bin)
install(DIRECTORY include/espeak include/espeak-ng TYPE INCLUDE)
