add_executable(jev-searchfs jev-searchfs.cpp)

target_link_libraries(jev-searchfs jevmachopp jevplatformize cxxopts fmt)
target_compile_definitions(jev-searchfs PRIVATE USE_FMT=1)
