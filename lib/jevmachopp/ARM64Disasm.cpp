#include "jevmachopp/ARM64Disasm.h"

namespace ARM64Disasm {

constexpr uint32_t bl_opc_mask = 0xfc000000;
constexpr uint32_t bl_opc_match = 0x94000000;

bool isBL(const inst_raw_t inst) {
    return (inst & bl_opc_mask) == bl_opc_match;
}

std::optional<bl_off> decodeBL(const inst_raw_t inst) {
    if (!isBL(inst)) {
        return {};
    }
    return (((inst & ~bl_opc_mask) << 6) >> 6);
}

bool isBLTo(const inst_raw_t inst, uint64_t pc, uint64_t target_addr) {
    return false;
}

} // namespace ARM64Disasm
