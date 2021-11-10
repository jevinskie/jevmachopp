
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

add_library(jevmachopp ${MACHOPP_SRC})

target_include_directories(jevmachopp PUBLIC ../../include)
