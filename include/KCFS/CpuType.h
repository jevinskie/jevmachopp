#pragma once

#include <cstdint>

enum class CpuType : std::int32_t {
#include <mach/machine.h>

  ANY = CPU_TYPE_ANY,
  X86 = CPU_TYPE_X86,
  I386 = CPU_TYPE_X86,
  X86_64 = CPU_TYPE_X86_64,
  ARM = CPU_TYPE_ARM,
  POWERPC = CPU_TYPE_POWERPC,
  POWERPC64 = CPU_TYPE_POWERPC64,
};
