cmake_minimum_required(VERSION 2.8.11)
project(Plane-Detection-Point-Cloud)

option(PDPC_USE_OMP "Use OpenMP" ON)

set(CMAKE_CXX_FLAGS                "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG          "-DPDPC_DEBUG -g3 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -O2 -g3")

# Enable helpfull warnings and C++14 for all files
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(PDPC_USE_OMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else()
        message(WARNING "OpenMP ON but not found!")
    endif()
endif()

find_package(CGAL REQUIRED)
set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE true)
set(CGAL_DISABLE_ROUNDING_MATH_CHECK true) # for valgrind ?

file(GLOB_RECURSE lib_headers src/*.hpp src/*.h)
file(GLOB_RECURSE lib_sources src/*.cpp)
file(GLOB_RECURSE lib_inlines src/*.inl)
set(lib_files ${lib_headers} ${lib_sources} ${lib_inlines})

add_library(pdpclib STATIC ${lib_files})
target_include_directories(pdpclib PUBLIC src)

file(GLOB_RECURSE app_sources app/*.cpp)
foreach(file ${app_sources})
    get_filename_component(name ${file} NAME_WE)
    message(STATUS "Build app ${name}")
    add_executable(${name} ${file})
    add_dependencies(${name} pdpclib)
    target_link_libraries(${name} pdpclib CGAL::CGAL)
    install(TARGETS ${name} DESTINATION bin)
endforeach()
