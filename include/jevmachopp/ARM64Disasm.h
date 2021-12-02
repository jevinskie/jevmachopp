#pragma once

#include "jevmachopp/Common.h"

namespace ARM64Disasm {

using inst_raw_t = uint32_t;
using bl_off = int32_t;

bool isBL(const inst_raw_t inst);

std::optional<bl_off> decodeBL(const inst_raw_t inst);

bool isBLTo(const inst_raw_t inst, uint64_t pc, uint64_t target_addr);

} // namespace ARM64Disasm

class ARM64Inst {
public:
    ARM64Inst(uint32_t inst_word, uint64_t pc) : inst_word(inst_word), pc(pc) {}

public:
    bool isBL() const;
    bool isBLTo(uint64_t target_addr) const;

private:
    const ARM64Disasm::inst_raw_t inst_word;
    const uint64_t pc;
};
