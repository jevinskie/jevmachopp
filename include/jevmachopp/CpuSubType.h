#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>

using jev_cpu_subtype_t = std::int32_t;

enum class CpuSubType : jev_cpu_subtype_t {
    ANY            = -1,
    VAX_ALL        = 0,
    VAX780         = 1,
    VAX785         = 2,
    VAX750         = 3,
    VAX730         = 4,
    UVAXI          = 5,
    UVAXII         = 6,
    VAX8200        = 7,
    VAX8500        = 8,
    VAX8600        = 9,
    VAX8650        = 10,
    VAX8800        = 11,
    UVAXIII        = 12,
    MC680x0_ALL    = 1,
    MC68030        = 1,
    MC68040        = 2,
    MC68030_ONLY   = 3,
    I386_ALL       = (3) + ((0) << 4),
    I386           = (3) + ((0) << 4),
    I486           = (4) + ((0) << 4),
    I486SX         = (4) + ((8) << 4),
    I586           = (5) + ((0) << 4),
    PENT           = (5) + ((0) << 4),
    PENTPRO        = (6) + ((1) << 4),
    PENTII_M3      = (6) + ((3) << 4),
    PENTII_M5      = (6) + ((5) << 4),
    CELERON        = (7) + ((6) << 4),
    CELERON_MOBILE = (7) + ((7) << 4),
    PENTIUM_3      = (8) + ((0) << 4),
    PENTIUM_3_M    = (8) + ((1) << 4),
    PENTIUM_3_XEON = (8) + ((2) << 4),
    PENTIUM_M      = (9) + ((0) << 4),
    PENTIUM_4      = (10) + ((0) << 4),
    PENTIUM_4_M    = (10) + ((1) << 4),
    ITANIUM        = (11) + ((0) << 4),
    ITANIUM_2      = (11) + ((1) << 4),
    XEON           = (12) + ((0) << 4),
    XEON_MP        = (12) + ((1) << 4),
    X86_ALL        = 3,
    X86_64_ALL     = 3,
    X86_ARCH1      = 4,
    X86_64_H       = 8,
    MIPS_ALL       = 0,
    MIPS_R2300     = 1,
    MIPS_R2600     = 2,
    MIPS_R2800     = 3,
    MIPS_R2000a    = 4,
    MIPS_R2000     = 5,
    MIPS_R3000a    = 6,
    MIPS_R3000     = 7,
    MC98000_ALL    = 0,
    MC98601        = 1,
    HPPA_ALL       = 0,
    HPPA_7100      = 0,
    HPPA_7100LC    = 1,
    MC88000_ALL    = 0,
    MC88100        = 1,
    MC88110        = 2,
    SPARC_ALL      = 0,
    I860_ALL       = 0,
    I860_860       = 1,
    POWERPC_ALL    = 0,
    POWERPC_601    = 1,
    POWERPC_602    = 2,
    POWERPC_603    = 3,
    POWERPC_603e   = 4,
    POWERPC_603ev  = 5,
    POWERPC_604    = 6,
    POWERPC_604e   = 7,
    POWERPC_620    = 8,
    POWERPC_750    = 9,
    POWERPC_7400   = 10,
    POWERPC_7450   = 11,
    POWERPC_970    = 100,
    ARM_ALL        = 0,
    ARM_V4T        = 5,
    ARM_V6         = 6,
    ARM_V5TEJ      = 7,
    ARM_XSCALE     = 8,
    ARM_V7         = 9,
    ARM_V7F        = 10,
    ARM_V7S        = 11,
    ARM_V7K        = 12,
    ARM_V8         = 13,
    ARM_V6M        = 14,
    ARM_V7M        = 15,
    ARM_V7EM       = 16,
    ARM_V8M        = 17,
    ARM64_ALL      = 0,
    ARM64_V8       = 1,
    ARM64E         = 2,
    ARM64_32_ALL   = 0,
    ARM64_32_V8    = 1,
};

#if USE_FMT
template <> struct fmt::formatter<CpuSubType> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CpuSubType const &subtype, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<CpuSubType ({:#010x})>"_cf, as_unsigned(subtype));
    }
};
#endif
