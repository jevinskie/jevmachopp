
set(MACHOPP_SRC
	EncryptionInfoCommand.cpp
	GenericCommand.cpp
	LoadCommand.cpp
	MachO.cpp
	Section.cpp
	SegmentCommand.cpp
	Slurp.cpp
	UUIDCommand.cpp
)

add_library(MACHOPP_SRC ${MACHOPP_SRC})

target_include_directories(MACHOPP_SRC PUBLIC ../../include)
