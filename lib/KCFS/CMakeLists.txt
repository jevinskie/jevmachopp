
set(KCFS_SRC
	EncryptionInfoCommand.cpp
	GenericCommand.cpp
	LoadCommand.cpp
	MachO.cpp
	Section.cpp
	SegmentCommand.cpp
	Slurp.cpp
	UUIDCommand.cpp
)

add_library(KCFS ${KCFS_SRC})

target_include_directories(KCFS PUBLIC ../../include)
