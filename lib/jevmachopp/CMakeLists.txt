
set(MACHOPP_SRC
    BuildVersionCommand.cpp
    DeviceTree.cpp
    DylibCommand.cpp
    DySymtabCommand.cpp
    FilesetEntryCommand.cpp
    Hex.cpp
    LoadCommand.cpp
    LoadSubCommand.cpp
    LinkeditDataCommand.cpp
    MachO.cpp
    MachOHeaderEnums.cpp
    NList.cpp
    NVRAM.cpp
    Section.cpp
    SegmentCommand.cpp
    Slurp.cpp
    SourceVersionCommand.cpp
    SymtabCommand.cpp
    Version.cpp
    UnknownCommand.cpp
)

set(MACHOPP_HDR_STANDALONE
    c/jevdtree.h
    c/jevmacho.h
    c/jevnvram.h
    Common.h
    CpuType.h
    CpuTypeMeta.h
    CpuSubType.h
    DelimitedCStr.h
    EncryptionInfoCommand.h
    LoadCommandType.h
    PackedCStr.h
    UUIDCommand.h
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

target_link_libraries(jevmachopp PUBLIC Boost::callable_traits enum.hpp fmt nanorange visit)
target_compile_definitions(jevmachopp PRIVATE USE_FMT=1)

target_include_directories(jevmachopp PUBLIC ../../include ../../3rdparty/static_string/include ../../3rdparty/static_vector/include ../../3rdparty/hedley)

# target_compile_options(jevmachopp PRIVATE -Xclang -fdump-record-layouts)
