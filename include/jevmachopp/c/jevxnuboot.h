#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace XNUBoot {
#endif

void load_and_prep_xnu_kernelcache(const void *boot_args_base);

__attribute__((noreturn)) void xnu_jump_stub(uint64_t new_base_plus_bootargs_off,
                                             uint64_t image_addr, uint64_t new_base,
                                             uint64_t image_size, uint64_t image_entry);
extern char _xnu_jump_stub_end;

#ifdef __cplusplus
} // namespace XNUBoot
#endif

#ifdef __cplusplus
} // extern "C"
#endif
