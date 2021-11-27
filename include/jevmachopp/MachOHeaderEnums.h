#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <enum.hpp/enum.hpp>
#include <experimental/fixed_capacity_vector>
#include <string>

// clang-format off
ENUM_HPP_CLASS_DECL(MachOFileType, std::uint32_t,
	(OBJECT = 0x1)
	(EXECUTE = 0x2)
	(FVMLIB = 0x3)
	(CORE = 0x4)
	(PRELOAD = 0x5)
	(DYLIB = 0x6)
	(DYLINKER = 0x7)
	(BUNDLE = 0x8)
	(DYLIB_STUB = 0x9)
	(DSYM = 0xa)
	(KEXT_BUNDLE = 0xb)
	(FILESET = 0xc)
)
// clang-format on

template <> struct fmt::formatter<MachOFileType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(MachOFileType const &fileType, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<MachOFileType {:s} ({:#010x})>"_cf,
                              MachOFileType_traits::to_string_or_empty(fileType),
                              as_unsigned(fileType));
    }
};

// clang-format off
ENUM_HPP_CLASS_DECL(MachOFlags, std::uint32_t,
	(NOUNDEFS = 0x1)
	(INCRLINK = 0x2)
	(DYLDLINK = 0x4)
	(BINDATLOAD = 0x8)
	(PREBOUND = 0x10)
	(SPLIT_SEGS = 0x20)
	(LAZY_INIT = 0x40)
	(TWOLEVEL = 0x80)
	(FORCE_FLAT = 0x100)
	(NOMULTIDEFS = 0x200)
	(NOFIXPREBINDING = 0x400)
	(PREBINDABLE = 0x800)
	(ALLMODSBOUND = 0x1000)
	(SUBSECTIONS_VIA_SYMBOLS = 0x2000)
	(CANONICAL = 0x4000)
	(WEAK_DEFINES = 0x8000)
	(BINDS_TO_WEAK = 0x10000)
	(ALLOW_STACK_EXECUTION = 0x20000)
	(ROOT_SAFE = 0x40000)
	(SETUID_SAFE = 0x80000)
	(NO_REEXPORTED_DYLIBS = 0x100000)
	(PIE = 0x200000)
	(DEAD_STRIPPABLE_DYLIB = 0x400000)
	(HAS_TLV_DESCRIPTORS = 0x800000)
	(NO_HEAP_EXECUTION = 0x1000000)
	(APP_EXTENSION_SAFE = 0x02000000)
	(NLIST_OUTOFSYNC_WITH_DYLDINFO = 0x04000000)
	(SIM_SUPPORT = 0x08000000)
	(DYLIB_IN_CACHE = 0x80000000)
)
// clang-format on

std::experimental::fixed_capacity_vector<std::string_view, 32> to_strings(MachOFlags const &flags);

template <> struct fmt::formatter<MachOFlags> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(MachOFlags const &flags, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<MachOFlags {} ({:#010x})>"_cf,
                              fmt::join(to_strings(flags), ", "), as_unsigned(flags));
    }
};
