#pragma once

#include "jevmachopp/Common.h"

#include "fmt/core.h"
#include <mach-o/loader.h>
#include <cstdint>
#include <enum.hpp/enum.hpp>
#include <experimental/fixed_capacity_vector>
#include <string>

// clang-format off
ENUM_HPP_CLASS_DECL(MachOFileType, std::uint32_t,
	(OBJECT = MH_OBJECT)
	(EXECUTE = MH_EXECUTE)
	(FVMLIB = MH_FVMLIB)
	(CORE = MH_CORE)
	(PRELOAD = MH_PRELOAD)
	(DYLIB = MH_DYLIB)
	(DYLINKER = MH_DYLINKER)
	(BUNDLE = MH_BUNDLE)
	(DYLIB_STUB = MH_DYLIB_STUB)
	(DSYM = MH_DSYM)
	(KEXT_BUNDLE = MH_KEXT_BUNDLE)
	(FILESET = MH_FILESET)
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
	(NOUNDEFS = MH_NOUNDEFS)
	(INCRLINK = MH_INCRLINK)
	(DYLDLINK = MH_DYLDLINK)
	(BINDATLOAD = MH_BINDATLOAD)
	(PREBOUND = MH_PREBOUND)
	(SPLIT_SEGS = MH_SPLIT_SEGS)
	(LAZY_INIT = MH_LAZY_INIT)
	(TWOLEVEL = MH_TWOLEVEL)
	(FORCE_FLAT = MH_FORCE_FLAT)
	(NOMULTIDEFS = MH_NOMULTIDEFS)
	(NOFIXPREBINDING = MH_NOFIXPREBINDING)
	(PREBINDABLE = MH_PREBINDABLE)
	(ALLMODSBOUND = MH_ALLMODSBOUND)
	(SUBSECTIONS_VIA_SYMBOLS = MH_SUBSECTIONS_VIA_SYMBOLS)
	(CANONICAL = MH_CANONICAL)
	(WEAK_DEFINES = MH_WEAK_DEFINES)
	(BINDS_TO_WEAK = MH_BINDS_TO_WEAK)
	(ALLOW_STACK_EXECUTION = MH_ALLOW_STACK_EXECUTION)
	(ROOT_SAFE = MH_ROOT_SAFE)
	(SETUID_SAFE = MH_SETUID_SAFE)
	(NO_REEXPORTED_DYLIBS = MH_NO_REEXPORTED_DYLIBS)
	(PIE = MH_PIE)
	(DEAD_STRIPPABLE_DYLIB = MH_DEAD_STRIPPABLE_DYLIB)
	(HAS_TLV_DESCRIPTORS = MH_HAS_TLV_DESCRIPTORS)
	(NO_HEAP_EXECUTION = MH_NO_HEAP_EXECUTION)
	(APP_EXTENSION_SAFE = MH_APP_EXTENSION_SAFE)
	(NLIST_OUTOFSYNC_WITH_DYLDINFO = MH_NLIST_OUTOFSYNC_WITH_DYLDINFO)
	(SIM_SUPPORT = MH_SIM_SUPPORT)
	(DYLIB_IN_CACHE = MH_DYLIB_IN_CACHE)
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
