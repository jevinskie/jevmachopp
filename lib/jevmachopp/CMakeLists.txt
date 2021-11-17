
set(MACHOPP_SRC
    Common.cpp
    CpuTypeMeta.cpp
    DylibCommand.cpp
    DySymtabCommand.cpp
    EncryptionInfoCommand.cpp
    LoadCommand.cpp
    LoadSubCommand.cpp
    LinkerDataCommand.cpp
    MachO.cpp
    NList.cpp
    Section.cpp
    SegmentCommand.cpp
    Slurp.cpp
    Strtab.cpp
    SymtabCommand.cpp
    UnknownCommand.cpp
    UUIDCommand.cpp
)

set(MACHOPP_HDR_STANDALONE
    CpuType.h
    CpuSubType.h
    LoadCommandType.h
)

set(MACHOPP_HDR)
foreach(HDR ${MACHOPP_HDR_STANDALONE})
    set(HDR "${CMAKE_CURRENT_SOURCE_DIR}/../../include/jevmachopp/${HDR}")
    list(APPEND MACHOPP_HDR ${HDR})
endforeach()


foreach(SRC ${MACHOPP_SRC})
    string(REGEX REPLACE "\.cpp$" ".h" HDR ${SRC})
    set(HDR "${CMAKE_CURRENT_SOURCE_DIR}/../../include/jevmachopp/${HDR}")
    list(APPEND MACHOPP_HDR ${HDR})
endforeach()


# MACHOPP_HDR added for Xcode project generation
add_library(jevmachopp ${MACHOPP_SRC} ${MACHOPP_HDR})

target_link_libraries(jevmachopp enum.hpp fmt range-v3-meta range-v3-concepts range-v3)

target_include_directories(jevmachopp PUBLIC ../../include ../../3rdparty/hedley)

# target_compile_options(jevmachopp PRIVATE -Xclang -fdump-record-layouts)
