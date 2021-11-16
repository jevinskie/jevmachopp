cmake_minimum_required(VERSION 3.21)

project(jevmachopp CXX)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

# add_compile_options(-Os -flto=full -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fvirtual-function-elimination -ffunction-sections -fdata-sections)
# add_link_options(-flto=full -Wl,-dead_strip)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty)

add_subdirectory(lib)
add_subdirectory(tools)

