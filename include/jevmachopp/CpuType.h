#pragma once

#include <cstdint>
#include <mach/machine.h>

enum class CpuType : std::int32_t {
    ANY = CPU_TYPE_ANY,
    X86 = CPU_TYPE_X86,
    I386 = CPU_TYPE_X86,
    X86_64 = CPU_TYPE_X86_64,
    ARM = CPU_TYPE_ARM,
    POWERPC = CPU_TYPE_POWERPC,
    POWERPC64 = CPU_TYPE_POWERPC64,
};
