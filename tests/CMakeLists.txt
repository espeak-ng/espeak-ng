include(CTest)

list(APPEND _binary_tests)

macro(compiled_test _test_name)
  add_executable(test_${_test_name}
    $<TARGET_OBJECTS:espeak-ng>
    ${_test_name}.c
  )
  target_link_libraries(test_${_test_name} PRIVATE
    $<TARGET_PROPERTY:espeak-ng,LINK_LIBRARIES>
  )
  target_include_directories(
    test_${_test_name} PRIVATE
    $<TARGET_PROPERTY:espeak-ng,SOURCE_DIR>
    $<TARGET_PROPERTY:espeak-ng,SOURCE_DIR>/include/compat
    $<TARGET_PROPERTY:espeak-ng,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:espeak-ng-config,INTERFACE_INCLUDE_DIRECTORIES>
  )
  add_dependencies(test_${_test_name} data)
  add_test(
    NAME ${_test_name}
    COMMAND ${ESPEAK_RUN_ENV} $<TARGET_FILE:test_${_test_name}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
  )
  list(APPEND _binary_tests test_${_test_name})
endmacro(compiled_test)

find_program(SHELL bash)

macro(shell_test _test_name)
  add_test(
    NAME ${_test_name}
    COMMAND ${ESPEAK_RUN_ENV} ESPEAK_BIN=$<TARGET_FILE:espeak-ng-bin> ${SHELL} ${CMAKE_CURRENT_SOURCE_DIR}/${_test_name}.test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
  )
endmacro(shell_test)

compiled_test(api)
compiled_test(encoding)
compiled_test(ieee80)
compiled_test(readclause)

if (SHELL AND UNIX)

# shell_test(bom)
# shell_test(non-executable-files-with-executable-bit)

shell_test(cmd_options)
shell_test(dictionary)
shell_test(language-numbers-cardinal)
shell_test(language-numbers-ordinal)
# shell_test(language-phonemes)
shell_test(language-pronunciation)
shell_test(language-replace)
shell_test(ssml)
shell_test(translate)
shell_test(variants)
shell_test(voices)

# shell_test(windows-data)
# shell_test(windows-installer)

if (USE_KLATT)
  shell_test(klatt)
endif()
if (USE_MBROLA)
  shell_test(mbrola)
endif()

endif()

add_custom_target(tests DEPENDS ${_binary_tests})
