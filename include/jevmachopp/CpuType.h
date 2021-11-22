#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <mach/machine.h>

#include <enum.hpp/enum.hpp>

using jev_cpu_type_t = std::int32_t;

// clang-format off
ENUM_HPP_CLASS_DECL(CpuType, jev_cpu_type_t,
    (ANY = -1)
    (VAX = 1)
    (MC680x0 = 6)
    (X86 = 7)
    // (I386 = X86) // dupe of X86
    (X86_64 = X86 | 0x01000000)
    (MC98000 = 10)
    (HPPA = 11)
    (ARM = 12)
    (ARM64 = ARM | 0x01000000)
    (ARM64_32 = ARM | 0x02000000)
    (MC88000 = 13)
    (SPARC = 14)
    (I860 = 15)
    (POWERPC = 18)
    (POWERPC64 = POWERPC | 0x01000000)
)
// clang-format on

template <> struct fmt::formatter<CpuType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CpuType const &type, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<CpuType {:s} ({:#010x})>"_cf,
                              CpuType_traits::to_string_or_empty(type), as_unsigned(type));
    }
};
