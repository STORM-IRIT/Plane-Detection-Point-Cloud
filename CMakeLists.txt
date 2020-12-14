cmake_minimum_required(VERSION 2.8.11)
project(Plane-Detection-Point-Cloud)

file(GLOB_RECURSE lib_headers src/*.hpp src/*.h)
file(GLOB_RECURSE lib_sources src/*.cpp)
file(GLOB_RECURSE lib_inlines src/*.inl)
set(lib_files ${lib_headers} ${lib_sources} ${lib_inlines})

add_library(pdpclib STATIC ${lib_files})
target_include_directories(pdpclib PUBLIC external/Eigen/ external/Ponca/src src/)


file(GLOB_RECURSE app_sources app/*.cpp)

foreach(file ${app_sources})
    get_filename_component(name ${file} NAME_WE)
    message(STATUS "Build app ${name}")
    add_executable(${name} ${file})
    add_dependencies(${name} pdpclib)
    target_link_libraries(${name} pdpclib)
endforeach()
