cmake_minimum_required(VERSION 2.8.11)
project(Plane-Detection-Point-Cloud)

file(GLOB_RECURSE headers src/*.h)
file(GLOB_RECURSE sources src/*.cpp)
set(source_files ${headers} ${sources})

include_directories(external/Eigen/ src/)

add_executable(hello ${source_files})
