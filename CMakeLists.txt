cmake_minimum_required(VERSION 3.10)

project(RetardPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(SOURCES
    src/main.cpp
    src/App.cpp
    src/MainMenuScene.cpp
    src/VideoPlayerScene.cpp
    src/AudioProcessor.cpp
    src/VideoRenderer.cpp
    src/UIManager.cpp
    src/UIButton.cpp
    src/UIElement.cpp
    src/VideoUI.cpp
    src/MediaProcessor.cpp
    src/VideoProcessor.cpp
    src/FileDialog.cpp
        src/ToolBar.cpp
        src/nativefiledialog/nfd_gtk.cpp
       # src/nativefiledialog/nfd_portal.cpp
        #src/nativefiledialog/nfd_cocoa.m
       # src/nativefiledialog/nfd_win.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)
pkg_check_modules(SWSRESAMPLE REQUIRED libswresample)

add_executable(RetardPlayer ${SOURCES})

target_link_libraries(RetardPlayer
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer   # Fix: Correct the library target name
    ${GTK3_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWSCALE_LIBRARIES}
    ${SWSRESAMPLE_LIBRARIES}
)

target_include_directories(RetardPlayer PRIVATE
        include
        ${GTK3_INCLUDE_DIRS}
)

target_compile_options(RetardPlayer PRIVATE -g)

