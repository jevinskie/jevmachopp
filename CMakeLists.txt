cmake_minimum_required(VERSION 3.19)

project(kernelcache-fileset-utils CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty/fmt)

add_subdirectory(lib)
add_subdirectory(tools)

