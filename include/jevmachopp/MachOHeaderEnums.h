#pragma once

#include "jevmachopp/Common.h"

#include "fmt/core.h"
#include <cstdint>
#include <enum.hpp/enum.hpp>
#include <mach-o/loader.h>

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
                              to_underlying_int(fileType));
    }
};
