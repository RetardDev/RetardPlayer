cmake_minimum_required(VERSION 3.28)

project(RetardPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/App.cpp
    src/MainMenuScene.cpp
    src/VideoPlayerScene.cpp
    src/VideoRenderer.cpp
    src/UIManager.cpp
    src/UIButton.cpp
    src/UIElement.cpp
    src/VideoUI.cpp
    src/VideoProcessor.cpp 
    src/FileDialog.cpp
  )

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

if (NOT SDL2_LIBRARIES)
    set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
endif()

if (NOT SDL2_TTF_LIBRARIES)
    set(SDL2_TTF_LIBRARIES SDL2_ttf::SDL2_ttf)
endif()

add_executable(RetardPlayer ${SOURCES})

target_link_libraries(RetardPlayer ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${GTK3_LIBRARIES})

target_compile_options(RetardPlayer PRIVATE -g)
