add_library(speechPlayer STATIC
  src/frame.cpp
  src/speechPlayer.cpp
  src/speechWaveGenerator.cpp
)
target_include_directories(speechPlayer PUBLIC include)
target_compile_options(speechPlayer PRIVATE "-fPIC")
install(TARGETS speechPlayer ARCHIVE)
