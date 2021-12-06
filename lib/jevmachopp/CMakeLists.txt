
set(MACHOPP_SRC
    ARM64Disasm.cpp
    BuildVersionCommand.cpp
    CallFinder.cpp
    CallStubs.cpp
    Common.cpp
    CommonTypes.cpp
    DeviceTree.cpp
    DylibCommand.cpp
    DySymtabCommand.cpp
    FatMachO.cpp
    FilesetEntryCommand.cpp
    FunctionStartsCommand.cpp
    Hex.cpp
    LEB128.cpp
    LoadCommand.cpp
    LoadSubCommand.cpp
    LinkeditDataCommand.cpp
    m1n1.cpp
    MachO.cpp
    MachOHeaderEnums.cpp
    NList.cpp
    NVRAM.cpp
    SearchFS.cpp
    Section.cpp
    SegmentCommand.cpp
    Slurp.cpp
    SourceVersionCommand.cpp
    SymtabCommand.cpp
    Version.cpp
    UnknownCommand.cpp
    XNUBoot.cpp
)

set(MACHOPP_HDR_STANDALONE
    c/jevdtree.h
    c/jevmacho.h
    c/jevnvram.h
    c/jevxnuboot.h
    CpuType.h
    CpuTypeMeta.h
    CpuSubType.h
    CSR.h
    DelimitedCStr.h
    EncryptionInfoCommand.h
    LoadCommandType.h
    PackedCStr.h
    Ranges.h
    RingBuffer.h
    UnixThreadCommand.h
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

list(APPEND MACHOPP_SRC
    m1n1_asm.S
)

# MACHOPP_HDR added for Xcode project generation
add_library(jevmachopp ${MACHOPP_SRC} ${MACHOPP_HDR})


target_link_libraries(jevmachopp PUBLIC bn-arm64-disasm Boost::callable_traits enum.hpp fmt nanorange uleb128 visit)
target_compile_definitions(jevmachopp PRIVATE USE_FMT=1)
target_compile_options(jevmachopp PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(jevmachopp PRIVATE -fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables)

set_source_files_properties(SearchFS.cpp PROPERTIES COMPILE_FLAGS -fexceptions)

target_include_directories(jevmachopp PUBLIC ../../include ../../3rdparty/static_string/include ../../3rdparty/static_vector/include ../../3rdparty/thread-pool ../../3rdparty/hedley ../../3rdparty/cpp17-semaphore ../../3rdparty/concurrent-resource)

# target_compile_options(jevmachopp PRIVATE -Xclang -fdump-record-layouts)


add_library(jevplatformize STATIC Platformize.cpp ../../include/jevmachopp/Platformize.h)
set_target_properties(jevplatformize PROPERTIES OSX_ARCHITECTURES arm64e)

add_library(jevplatformize-helper SHARED PlatformizeHelper.cpp)
set_target_properties(jevplatformize-helper PROPERTIES OSX_ARCHITECTURES arm64e)

