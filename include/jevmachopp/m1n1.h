#pragma once

#include "jevmachopp/Common.h"

extern "C" __attribute__((noreturn)) void xnu_jump_stub(uint64_t new_base_plus_bootargs_off,
                                                        uint64_t image_addr, uint64_t new_base,
                                                        uint64_t image_size, uint64_t image_entry) {

}
