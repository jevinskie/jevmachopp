cmake_minimum_required(VERSION 3.21)

project(jevmachopp CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty)

add_subdirectory(lib)
add_subdirectory(tools)

