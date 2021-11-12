
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

set(MACHOPP_HDR)
foreach(SRC ${MACHOPP_SRC})
    string(REGEX REPLACE "\.cpp$" ".h" HDR ${SRC})
    set(HDR "${CMAKE_CURRENT_SOURCE_DIR}/../../include/jevmachopp/${HDR}")
    list(APPEND MACHOPP_HDR ${HDR})
endforeach()


# MACHOPP_HDR added for Xcode project generation
add_library(jevmachopp ${MACHOPP_SRC} ${MACHOPP_HDR})

target_link_libraries(jevmachopp fmt magic_enum)

target_include_directories(jevmachopp PUBLIC ../../include)

# target_compile_options(jevmachopp PRIVATE -Xclang -fdump-record-layouts)
