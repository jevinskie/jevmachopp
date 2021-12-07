add_executable(jev-ringbuf jev-ringbuf.cpp)

target_link_libraries(jev-ringbuf jevmachopp nanobench benchmark fmt)
target_compile_definitions(jev-ringbuf PRIVATE USE_FMT=1)
