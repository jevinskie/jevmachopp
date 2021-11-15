#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <mach/machine.h>

#include <magic_enum.hpp>

enum class CpuType : std::int32_t {
    ANY = CPU_TYPE_ANY,
    X86 = CPU_TYPE_X86,
    I386 = CPU_TYPE_X86,
    X86_64 = CPU_TYPE_X86_64,
    ARM = CPU_TYPE_ARM,
    POWERPC = CPU_TYPE_POWERPC,
    POWERPC64 = CPU_TYPE_POWERPC64,
};

template <> struct fmt::formatter<CpuType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CpuType const &type, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<CpuType {:s} ({:08x})>", magic_enum::enum_name(type),
                              (uint32_t)to_underlying_int(type));
    }
};
