cmake_minimum_required(VERSION 3.22)

project(jevmachopp CXX C ASM)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
       add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

add_compile_definitions(FMT_USE_USER_DEFINED_LITERALS=1 FMT_USE_NONTYPE_TEMPLATE_PARAMETERS=1 FMT_USE_INT128=0 FMT_EXCEPTIONS=0 FMT_USE_FLOAT=0 FMT_USE_DOUBLE=0 FMT_USE_LONG_DOUBLE=0)
add_compile_definitions(BOOST_STATIC_STRING_STANDALONE=1 BOOST_NO_EXCEPTIONS=1)

add_compile_options(-Os -flto=full -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fvirtual-function-elimination -ffunction-sections -fdata-sections)
add_link_options(-Os -flto=full -Wl,-dead_strip)

# lol just don't its so bad
# add_compile_options(-fno-inline)
# add_link_options(-fno-inline)

add_compile_options(-ferror-limit=100)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty)

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)

