#include "jevmachopp/XNUBoot.h"
#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/NVRAM.h"

namespace XNUBoot {

const void *load_and_prep_xnu_kernelcache(const void *boot_args_base) {
    const void *entry_addr = nullptr;
    if (!boot_args_base) {
        return nullptr;
    }
    const auto &bootArgs = *(const XNUBootArgs *)boot_args_base;
    const auto virtBase = bootArgs.virtBase;
    const auto physBase = bootArgs.physBase;
    const auto virtOff = virtBase - physBase;
    FMT_PRINT("bootArgs.commandLine: {:s}\n", bootArgs.commandLine);
    // const auto &kcm = *(const MachO *)kernelcache_base;
    // FMT_PRINT("kcm: {}\n", kcm);
    // const auto kcm_vmaddr_range = kcm.vmaddr_range();
    // FMT_PRINT("kcm_vmaddr_range: {}\n", kcm_vmaddr_range);

    const auto *dt_phys_ptr = (const DTNode *)((uintptr_t)bootArgs.deviceTree - virtOff);
    const auto &dt = *dt_phys_ptr;

    if (auto chosen_node_ptr = dt.childNamed("chosen")) {
        auto &chosen_node = *chosen_node_ptr;

        if (auto iuou_prop_ptr = chosen_node.propertyNamed("internal-use-only-unit")) {
            auto &iuou_prop = *iuou_prop_ptr;
            printf("iuou prop before: 0x%08x\n", iuou_prop.as_u32());
            assert(iuou_prop.size_raw() == 4);
            (uint32_t &)iuou_prop.as_u32() = 1;
            printf("iuou prop after: 0x%08x\n", iuou_prop.as_u32());
        } else {
            printf("internal-use-only-unit prop not found in device tree\n");
            // sad but not fatal
            // return nullptr;
        }

        if (auto nvram_proxy_data_prop_ptr = chosen_node.propertyNamed("nvram-proxy-data")) {
            auto &nvram_proxy_data_prop = *nvram_proxy_data_prop_ptr;
            const auto proxyData = NVRAM::extractProxyData(nvram_proxy_data_prop.data());
            const auto hasVerbose = proxyData.system_hasBootArg("-v");
            printf("boot-args has -v flag: %d\n", hasVerbose);
            if (!hasVerbose) {
                printf("didn't detect verbose boot flag, bailing out of xnu load\n");
                return nullptr;
            }
        }

        if (auto memory_map_node_ptr = chosen_node.childNamed("memory-map")) {
            auto &memory_map_node = *memory_map_node_ptr;
            FMT_PRINT("memory_map_node: {}\n", memory_map_node);
            for (const auto &map_region : memory_map_node.properties_sized(sizeof(DTRegister))) {
                FMT_PRINT("map_region: {}\n", map_region);
                FMT_PRINT("map_region.as_reg(): {}\n", map_region.as_reg());
            }
        } else {
            printf("didn't detect verbose boot flag, bailing out of xnu load\n");
            return nullptr;
        }
    } else {
        printf("chosen node not found in device tree\n");
        return nullptr;
    }

    const auto cpuImplRegAddrs = DT::getCPUImplRegAddrs(dt);
    for (const auto &cpuImplRegAddr : cpuImplRegAddrs) {
        printf("cpuImplRegAddr: %p\n", (const void *)cpuImplRegAddr);
    }

    return entry_addr;
}

} // namespace XNUBoot
