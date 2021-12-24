#include "jevmachopp/ARM64Disasm.h"

namespace ARM64Disasm {

constexpr uint32_t bl_opc_mask  = 0xfc000000;
constexpr uint32_t bl_opc_match = 0x94000000;

bool isBL(const inst_raw_t inst) {
    return (inst & bl_opc_mask) == bl_opc_match;
}

constexpr uint32_t mov_wide_imm_opc_mask  = 0x1f800000;
constexpr uint32_t mov_wide_imm_opc_match = 0x12800000;
constexpr uint32_t mov_wide_imm_is64_mask = 0x80000000;
constexpr uint32_t mov_wide_imm_reg_mask  = 0x0000001f;
constexpr uint32_t mov_wide_imm_hw_mask   = 0x3;
constexpr uint32_t mov_wide_imm_hw_shift  = 21;
constexpr uint32_t mov_wide_imm_imm_mask  = 0xffff;
constexpr uint32_t mov_wide_imm_imm_shift = 5;

bool isMovWideImm(const inst_raw_t inst) {
    return (inst & mov_wide_imm_opc_mask) == mov_wide_imm_opc_match;
}

bool isMovWideImmTo(const inst_raw_t inst, uint8_t reg) {
    return isMovWideImm(inst) && decodeMovWideImm_reg(inst) == reg;
}

std::optional<bl_off> decodeBL(const inst_raw_t inst) {
    if (!isBL(inst)) {
        return {};
    }
    return ((((bl_off)(inst & ~bl_opc_mask)) << 6) >> 4);
}

uint8_t decodeMovWideImm_reg(const inst_raw_t inst) {
    return inst & mov_wide_imm_reg_mask;
}

std::optional<MovImmInfo> decodeMovWideImm(const inst_raw_t inst) {
    if (!isMovWideImm(inst)) {
        return {};
    }
    const bool is64        = inst & mov_wide_imm_is64_mask;
    const uint8_t reg      = decodeMovWideImm_reg(inst);
    const uint8_t hw       = (inst >> mov_wide_imm_hw_shift) & mov_wide_imm_hw_mask;
    const uint16_t raw_imm = (inst >> mov_wide_imm_imm_shift) & mov_wide_imm_imm_mask;
    const uint64_t imm     = raw_imm << (16 * hw);
    return MovImmInfo{imm, reg, is64};
}

bool isBLTo(const inst_raw_t inst, uint64_t pc, uint64_t target_addr) {
    if (auto bl_off = decodeBL(inst)) {
        return *bl_off + pc == target_addr;
    } else {
        return false;
    }
}

} // namespace ARM64Disasm

using namespace ARM64Disasm;

bool ARM64Inst::isBL() const {
    return ::isBL(inst_word);
}

bool ARM64Inst::isBLTo(uint64_t target_addr) const {
    return ::isBLTo(inst_word, pc, target_addr);
}