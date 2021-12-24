#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
namespace XNUBoot {
#endif

void load_and_prep_xnu_kernelcache(const void *boot_args_base);

#ifdef __cplusplus
} // namespace XNUBoot
#endif

#ifdef __cplusplus
} // extern "C"
#endif
