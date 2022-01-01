add_executable(jev-apfs-dump jev-apfs-dump.cpp)

target_link_libraries(jev-apfs-dump jevmachopp apfs_static)
