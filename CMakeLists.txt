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
  libs/rtmidi
  libs/rtaudio
  libs/AudioFile
  include/
  resources/
  libs/PluginBase/include
  libs/PluginBase/libs/json/include
)

set_target_properties(PluginBase PROPERTIES LINKER_LANGUAGE CXX)

source_group(TREE ${SRC} FILES ${SOURCE})

target_precompile_headers(SoundMixr PUBLIC
  "${SRC}include/pch.hpp"
)

target_link_libraries(SoundMixr
  GuiCode
  portaudio
  rtmidi
)

file(GLOB files "plugins/*.cpp")
foreach(file ${files})
  get_filename_component(pluginname ${file} NAME_WE)
  add_library(${pluginname} SHARED 
	${file}
  )
  
  target_include_directories(${pluginname} PUBLIC
    libs/PluginBase/include
    libs/PluginBase/libs/json/include
  )
  
  set_target_properties(${pluginname} PROPERTIES FOLDER "Plugins")
  
endforeach()


add_definitions(-DABSOLUTE_ASSETS_PATH=\"${SRCGUI}assets/\")