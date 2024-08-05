cmake_minimum_required(VERSION 3.28)
project(RetardPlayer)

set(CMAKE_CXX_STANDARD 17)
#find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_LIBRARIES})

add_executable(RetardPlayer main.cpp VideoRenderer.cpp)
target_link_libraries(RetardPlayer ${SDL2_LIBRARIES})

