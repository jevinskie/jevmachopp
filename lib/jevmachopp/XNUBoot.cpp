#include "jevmachopp/XNUBoot.h"
#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/NVRAM.h"

namespace XNUBoot {

const void *load_and_prep_xnu_kernelcache(const void *boot_args_base,
                                          const void *kernelcache_base) {
    const void *entry_addr = nullptr;
    if (!boot_args_base || !kernelcache_base) {
        return nullptr;
    }
    const auto &bootArgs = *(const XNUBootArgs *)boot_args_base;
    const auto virtBase = bootArgs.virtBase;
    const auto physBase = bootArgs.physBase;
    const auto virtOff = virtBase - physBase;
    FMT_PRINT("bootArgs.commandLine: {:s}\n", bootArgs.commandLine);
    const auto &kcm = *(const MachO *)kernelcache_base;
    // FMT_PRINT("kcm: {}\n", kcm);
    const auto kcm_vmaddr_range = kcm.vmaddr_range();
    FMT_PRINT("kcm_vmaddr_range: {}\n", kcm_vmaddr_range);
    const auto kcm_physddr_range = kcm_vmaddr_range - virtOff;
    FMT_PRINT("kcm_physddr_range: {}\n", kcm_physddr_range);

    const auto *dt_phys_ptr = (const DTNode *)((uintptr_t)bootArgs.deviceTree - virtOff);
    const auto &dt = *dt_phys_ptr;

    // FMT_PRINT("dt: {}\n", dt);

    return entry_addr;
}

} // namespace XNUBoot
