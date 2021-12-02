#pragma once

#include "jevmachopp/Common.h"

namespace ARM64Disasm {

using inst_raw_t = uint32_t;
using bl_off = int32_t;

bool isBL(const inst_raw_t inst);

std::optional<bl_off> decodeBL(const inst_raw_t inst);

bool isBLTo(const inst_raw_t inst, uint64_t pc, uint64_t target_addr);

} // namespace ARM64Disasm