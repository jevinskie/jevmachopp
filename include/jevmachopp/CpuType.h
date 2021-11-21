#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <mach/machine.h>

#include <enum.hpp/enum.hpp>

// clang-format off
ENUM_HPP_CLASS_DECL(CpuType, std::int32_t,
    (ANY = CPU_TYPE_ANY)
    (VAX = CPU_TYPE_VAX)
    (MC680x0 = CPU_TYPE_MC680x0)
    (X86 = CPU_TYPE_X86)
    // (I386 = CPU_TYPE_X86) // dupe of X86
    (X86_64 = CPU_TYPE_X86_64)
    (MC98000 = CPU_TYPE_MC98000)
    (HPPA = CPU_TYPE_HPPA)
    (ARM = CPU_TYPE_ARM)
    (ARM64 = CPU_TYPE_ARM64)
    (ARM64_32 = CPU_TYPE_ARM64_32)
    (MC88000 = CPU_TYPE_MC88000)
    (SPARC = CPU_TYPE_SPARC)
    (I860 = CPU_TYPE_I860)
    (POWERPC = CPU_TYPE_POWERPC)
    (POWERPC64 = CPU_TYPE_POWERPC64)
)
// clang-format on

template <> struct fmt::formatter<CpuType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CpuType const &type, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<CpuType {:s} ({:#010x})>",
                              CpuType_traits::to_string_or_empty(type), as_unsigned(type));
    }
};
