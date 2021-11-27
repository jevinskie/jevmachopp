#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <enum.hpp/enum.hpp>

using load_command_type_t = std::uint32_t;

constexpr load_command_type_t JEV_LC_REQ_DYLD = 0x80000000u;

// clang-format off
ENUM_HPP_CLASS_DECL(LoadCommandType, load_command_type_t,
    (SEGMENT = 0x1)
    (SYMTAB = 0x2)
    (SYMSEG = 0x3)
    (THREAD = 0x4)
    (UNIXTHREAD = 0x5)
    (LOADFVMLIB = 0x6)
    (IDFVMLIB = 0x7)
    (IDENT = 0x8)
    (FVMFILE = 0x9)
    (PREPAGE = 0xa)
    (DYSYMTAB = 0xb)
    (LOAD_DYLIB = 0xc)
    (ID_DYLIB = 0xd)
    (LOAD_DYLINKER = 0xe)
    (ID_DYLINKER = 0xf)
    (PREBOUND_DYLIB = 0x10)
    (ROUTINES = 0x11)
    (SUB_FRAMEWORK = 0x12)
    (SUB_UMBRELLA = 0x13)
    (SUB_CLIENT = 0x14)
    (SUB_LIBRARY = 0x15)
    (TWOLEVEL_HINTS = 0x16)
    (PREBIND_CKSUM = 0x17)
    (LOAD_WEAK_DYLIB = 0x18 | JEV_LC_REQ_DYLD)
    (SEGMENT_64 = 0x19)
    (ROUTINES_64 = 0x1a)
    (UUID = 0x1b)
    (RPATH = 0x1c | JEV_LC_REQ_DYLD)
    (CODE_SIGNATURE = 0x1d)
    (SEGMENT_SPLIT_INFO = 0x1e)
    (REEXPORT_DYLIB = 0x1f | JEV_LC_REQ_DYLD)
    (LAZY_LOAD_DYLIB = 0x20)
    (ENCRYPTION_INFO = 0x21)
    (DYLD_INFO = 0x22)
    (DYLD_INFO_ONLY = 0x22 | JEV_LC_REQ_DYLD)
    (LOAD_UPWARD_DYLIB = 0x23 | JEV_LC_REQ_DYLD)
    (VERSION_MIN_MACOSX = 0x24)
    (VERSION_MIN_IPHONEOS = 0x25)
    (FUNCTION_STARTS = 0x26)
    (DYLD_ENVIRONMENT = 0x27)
    (MAIN = 0x28 | JEV_LC_REQ_DYLD)
    (DATA_IN_CODE = 0x29)
    (SOURCE_VERSION = 0x2A)
    (DYLIB_CODE_SIGN_DRS = 0x2B)
    (ENCRYPTION_INFO_64 = 0x2C)
    (LINKER_OPTION = 0x2D)
    (LINKER_OPTIMIZATION_HINT = 0x2E)
    (VERSION_MIN_TVOS = 0x2F)
    (VERSION_MIN_WATCHOS = 0x30)
    (NOTE = 0x31)
    (BUILD_VERSION = 0x32)
    (DYLD_EXPORTS_TRIE = 0x33 | JEV_LC_REQ_DYLD)
    (DYLD_CHAINED_FIXUPS = 0x34 | JEV_LC_REQ_DYLD)
    (FILESET_ENTRY = 0x35 | JEV_LC_REQ_DYLD)
)
// clang-format on

template <> struct fmt::formatter<LoadCommandType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadCommandType const &lct, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<LoadCommandType {:s} ({:#010x})>"_cf,
                              LoadCommandType_traits::to_string_or_empty(lct),
                              to_underlying_int(lct));
    }
};
