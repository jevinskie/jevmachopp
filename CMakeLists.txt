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

add_compile_definitions(FMT_USE_USER_DEFINED_LITERALS=1 FMT_USE_NONTYPE_TEMPLATE_PARAMETERS=1 FMT_USE_INT128=0 FMT_EXCEPTIONS=0 FMT_USE_FLOAT=0 FMT_USE_DOUBLE=0 FMT_USE_LONG_DOUBLE=0)

add_compile_options(-Oz -flto=full -fvisibility=hidden -fvisibility-inlines-hidden -fwhole-program-vtables -fvirtual-function-elimination -ffunction-sections -fdata-sections)
add_link_options(-flto=full -Wl,-dead_strip)
add_compile_options(-fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables)

add_compile_options(-ferror-limit=100)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty)

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)

