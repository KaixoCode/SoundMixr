cmake_minimum_required (VERSION 3.0)
project (SoundMixr)

set (CMAKE_CXX_STANDARD 17)

add_subdirectory(libs)

set(SRC "${SoundMixr_SOURCE_DIR}/")

file(GLOB_RECURSE SOURCE
  "${SRC}source/*.cpp"
  "${SRC}include/*.hpp"
  "${SRC}resources/*.h"
  "${SRC}resources/*.rc"
)

add_executable(SoundMixr
  ${SOURCE}
)

target_include_directories(SoundMixr PUBLIC
  libs/GuiCode/include
  libs/GuiCode/libs
  libs/portaudio/include
  libs/AudioFile
  include/
  resources/
  libs/EffectBase/include
  libs/EffectBase/libs/json/include
)

set_target_properties(EffectBase PROPERTIES LINKER_LANGUAGE CXX)

source_group(TREE ${SRC} FILES ${SOURCE})

target_precompile_headers(SoundMixr PUBLIC
  "${SRC}include/pch.hpp"
)

target_link_libraries(SoundMixr
  GuiCode
  portaudio
)

file(GLOB files "effects/*.cpp")
foreach(file ${files})
  get_filename_component(effectname ${file} NAME_WE)
  add_library(${effectname} SHARED 
	${file}
  )
  
  target_include_directories(${effectname} PUBLIC
    libs/EffectBase/include
    libs/EffectBase/libs/json/include
  )
  
  set_target_properties(${effectname} PROPERTIES FOLDER "Effects")
  
endforeach()

add_definitions(-DABSOLUTE_ASSETS_PATH=\"${SRCGUI}assets/\")