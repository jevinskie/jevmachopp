add_subdirectory(jev-apfs-dump)
add_subdirectory(jev-dtree-dump)
add_subdirectory(jev-macho-callfinder)
add_subdirectory(jev-macho-dump)
add_subdirectory(jev-nvram-dump)
add_subdirectory(jev-ringbuf)

if (APPLE)
	add_subdirectory(jev-searchfs)
endif()

add_subdirectory(jev-xnu-load)
