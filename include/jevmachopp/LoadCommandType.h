#pragma once

#include "jevmachopp/Common.h"

#include "fmt/core.h"
#include "magic_enum.hpp"
#include <cstdint>
#include <mach-o/loader.h>

enum class LoadCommandType : std::uint32_t {
    SEGMENT = LC_SEGMENT,
    SYMTAB = LC_SYMTAB,
    SYMSEG = LC_SYMSEG,
    THREAD = LC_THREAD,
    UNIXTHREAD = LC_UNIXTHREAD,
    LOADFVMLIB = LC_LOADFVMLIB,
    IDFVMLIB = LC_IDFVMLIB,
    IDENT = LC_IDENT,
    FVMFILE = LC_FVMFILE,
    PREPAGE = LC_PREPAGE,
    DYSYMTAB = LC_DYSYMTAB,
    LOAD_DYLIB = LC_LOAD_DYLIB,
    ID_DYLIB = LC_ID_DYLIB,
    LOAD_DYLINKER = LC_LOAD_DYLINKER,
    ID_DYLINKER = LC_ID_DYLINKER,
    PREBOUND_DYLIB = LC_PREBOUND_DYLIB,
    ROUTINES = LC_ROUTINES,
    SUB_FRAMEWORK = LC_SUB_FRAMEWORK,
    SUB_UMBRELLA = LC_SUB_UMBRELLA,
    SUB_CLIENT = LC_SUB_CLIENT,
    SUB_LIBRARY = LC_SUB_LIBRARY,
    TWOLEVEL_HINTS = LC_TWOLEVEL_HINTS,
    PREBIND_CKSUM = LC_PREBIND_CKSUM,
    LOAD_WEAK_DYLIB = LC_LOAD_WEAK_DYLIB,
    SEGMENT_64 = LC_SEGMENT_64,
    ROUTINES_64 = LC_ROUTINES_64,
    UUID = LC_UUID,
    RPATH = LC_RPATH,
    CODE_SIGNATURE = LC_CODE_SIGNATURE,
    SEGMENT_SPLIT_INFO = LC_SEGMENT_SPLIT_INFO,
    REEXPORT_DYLIB = LC_REEXPORT_DYLIB,
    LAZY_LOAD_DYLIB = LC_LAZY_LOAD_DYLIB,
    ENCRYPTION_INFO = LC_ENCRYPTION_INFO,
    DYLD_INFO = LC_DYLD_INFO,
    DYLD_INFO_ONLY = LC_DYLD_INFO_ONLY,
    LOAD_UPWARD_DYLIB = LC_LOAD_UPWARD_DYLIB,
    VERSION_MIN_MACOSX = LC_VERSION_MIN_MACOSX,
    VERSION_MIN_IPHONEOS = LC_VERSION_MIN_IPHONEOS,
    FUNCTION_STARTS = LC_FUNCTION_STARTS,
    DYLD_ENVIRONMENT = LC_DYLD_ENVIRONMENT,
    MAIN = LC_MAIN,
    DATA_IN_CODE = LC_DATA_IN_CODE,
    SOURCE_VERSION = LC_SOURCE_VERSION,
    DYLIB_CODE_SIGN_DRS = LC_DYLIB_CODE_SIGN_DRS,
};

template <> struct fmt::formatter<LoadCommandType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadCommandType const &lct, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<LoadCommandType {:s} ({:#08x})>",
                              magic_enum::enum_name(lct), to_underlying_int(lct));
    }
};
