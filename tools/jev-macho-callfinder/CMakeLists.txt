add_executable(jev-macho-callfinder jev-macho-callfinder.cpp)

target_link_libraries(jev-macho-callfinder jevmachopp cxxopts fmt)
target_compile_definitions(jev-macho-callfinder PRIVATE USE_FMT=1)
