add_executable(jev-apfs-dump jev-apfs-dump.cpp)

target_link_libraries(jev-apfs-dump jevmachopp apfs_static)
target_compile_options(
    jev-apfs-dump
    PRIVATE
    "SHELL:-idirafter $ENV{UBOOT_INCLUDE_DIR}"
    "SHELL:-idirafter $ENV{UBOOT_INCLUDE_DIR}/../arch/sandbox/include"
    "-DCONFIG_IS_ENABLED(x)=1"
)
