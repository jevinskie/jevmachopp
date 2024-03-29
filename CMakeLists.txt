cmake_minimum_required(VERSION 3.22)

project(jevmachopp CXX C ASM)

# set(CMAKE_OSX_ARCHITECTURES arm64e)
set(CMAKE_XCODE_ATTRIBUTE_COMPILER_INDEX_STORE_ENABLE NO)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
       add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

if (CMAKE_CROSSCOMPILING)
add_compile_definitions(JEV_BAREMETAL)
# newlib locale hell
add_compile_definitions(_POSIX_C_SOURCE=200809 _XOPEN_SOURCE=700 _BSD_SOURCE _DEFAULT_SOURCE _GNU_SOURCE)
endif()

# add_compile_options(-Os -fvisibility=hidden -fvisibility-inlines-hidden -ffunction-sections -fdata-sections)
# add_link_options(-Os -Wl,-dead_strip)

add_compile_options(-O0 -ggdb3 -fno-omit-frame-pointer)
add_link_options(-O0 -ggdb3 -fno-omit-frame-pointer)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# add_compile_options(-fsanitize=memory)
# add_link_options(-fsanitize=memory)

# add_compile_options(-flto=full -fwhole-program-vtables -fvirtual-function-elimination)
# add_link_options(-flto=full)


# lol just don't its so bad
# add_compile_options(-fno-inline)
# add_link_options(-fno-inline)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
   add_compile_options(-ferror-limit=100)
endif()

add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,Clang>:-stdlib=libc++>)
add_link_options($<$<COMPILE_LANG_AND_ID:CXX,Clang>:-stdlib=libc++>)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS YES)

option(FMT_DOC OFF)
option(FMT_TEST OFF)
add_subdirectory(3rdparty)

add_subdirectory(lib)
if (NOT CMAKE_CROSSCOMPILING)
add_subdirectory(tools)
add_subdirectory(test)
endif()

