#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace XNUBoot {
#endif

void load_and_prep_xnu_kernelcache(const void *boot_args_base, const void *xnu_jump_stub_ptr,
                                   size_t stub_size);

#ifdef __cplusplus
} // namespace XNUBoot
#endif

#ifdef __cplusplus
} // extern "C"
#endif
