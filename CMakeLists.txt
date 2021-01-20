cmake_minimum_required (VERSION 3.0)
project (SoundMixr)

set (CMAKE_CXX_STANDARD 17)

add_subdirectory(libs)

set(SRC "${SoundMixr_SOURCE_DIR}/")

file(GLOB_RECURSE SOURCE
  "${SRC}source/*.cpp"
  "${SRC}include/*.hpp"
)

include_directories(
  libs/GuiCode/include
  libs/GuiCode/libs
  libs/rtsound/include/
  include/
)

add_executable(SoundMixr
  ${SOURCE}
)

source_group(TREE ${SRC} FILES ${SOURCE})

target_precompile_headers(SoundMixr PUBLIC
  "${SRC}include/pch.hpp"
)

target_link_libraries(SoundMixr
  rtaudio
  GuiCode
)