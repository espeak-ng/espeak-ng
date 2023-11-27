add_library(espeak-include INTERFACE)
target_include_directories(espeak-include INTERFACE include include/compat)

add_subdirectory(ucd-tools)
add_subdirectory(speechPlayer)
add_subdirectory(libespeak-ng)

add_executable(espeak-ng-bin espeak-ng.c)
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
    COMMAND ${CMAKE_COMMAND} -E create_symlink espeak-ng ${CMAKE_CURRENT_BINARY_DIR}/speak-ng
    COMMAND ${CMAKE_COMMAND} -E create_symlink espeak-ng ${CMAKE_CURRENT_BINARY_DIR}/espeak
    COMMAND ${CMAKE_COMMAND} -E create_symlink espeak-ng ${CMAKE_CURRENT_BINARY_DIR}/speak
    COMMENT "Link espeak-ng to compat names"
    DEPENDS espeak-ng-bin
  )
endif()
install(TARGETS espeak-ng-bin)
install(DIRECTORY include/espeak include/espeak-ng TYPE INCLUDE)

if (ESPEAK_COMPAT AND NOT WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/speak-ng DESTINATION bin)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/espeak DESTINATION bin)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/speak DESTINATION bin)
endif()
