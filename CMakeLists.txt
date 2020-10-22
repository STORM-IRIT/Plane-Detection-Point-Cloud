cmake_minimum_required(VERSION 2.8.11)
project(Plane-Detection-Point-Cloud)

file(GLOB_RECURSE headers src/*.h)
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE inlines src/*.inl)
set(source_files ${headers} ${sources} ${inlines})

include_directories(external/Eigen/ external/Ponca/src src/)

add_executable(hello ${source_files})
