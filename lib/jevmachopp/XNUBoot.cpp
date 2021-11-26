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
    FMT_PRINT("bootArgs.commandLine: {:s}\n", bootArgs.commandLine);
    const auto &kcm = *(const MachO *)kernelcache_base;
    FMT_PRINT("kcm: {}\n", kcm);
    return entry_addr;
}

} // namespace XNUBoot
