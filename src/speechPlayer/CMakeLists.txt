add_library(speechPlayer STATIC
  src/frame.cpp
  src/speechPlayer.cpp
  src/speechWaveGenerator.cpp
)
target_include_directories(speechPlayer PUBLIC include)
if(NOT MSVC)
  target_compile_options(speechPlayer PRIVATE "-fPIC")
endif()
install(TARGETS speechPlayer ARCHIVE)
