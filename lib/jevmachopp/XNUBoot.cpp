#include "jevmachopp/XNUBoot.h"
#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/NVRAM.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/UnixThreadCommand.h"
#include "jevmachopp/m1n1.h"

namespace XNUBoot {

void load_and_prep_xnu_kernelcache(const void *boot_args_base, const void *xnu_jump_stub_ptr,
                                   size_t stub_size) {
    if (!boot_args_base) {
        printf("boot_args_base is NULL\n");
        return;
    }
#ifdef M1N1
    if (!xnu_jump_stub_ptr) {
        printf("xnu_jump_stub_ptr is NULL\n");
        return;
    }
#endif
    const auto &bootArgs = *(const XNUBootArgs *)boot_args_base;
    const auto virtBase = bootArgs.virtBase;
    const auto physBase = bootArgs.physBase;
    const auto virtOff = virtBase - physBase;
    FMT_PRINT("bootArgs.commandLine: {:s} virtBase: {:p} physBase: {:p} virtOff: {:p}\n",
              bootArgs.commandLine, (void *)virtBase, (void *)physBase, (void *)virtOff);
    printf("xnu_jump_stub_ptr: %p\n", xnu_jump_stub_ptr);

    const auto *dt_phys_ptr = (const DTNode *)((uintptr_t)bootArgs.deviceTree - virtOff);
    const auto &dt = *dt_phys_ptr;

    auto chosen_node_ptr = dt.childNamed("chosen");
    if (!chosen_node_ptr) {
        printf("chosen node not found in device tree\n");
        return;
    }
    auto &chosen_node = *chosen_node_ptr;

    auto nvram_proxy_data_prop_ptr = chosen_node.propertyNamed("nvram-proxy-data");
    if (!nvram_proxy_data_prop_ptr) {
        printf("couldn't find chosen/nvram-proxy-data to check for verbose boot flag, bailing out "
               "of xnu load\n");
        return;
    }
    auto &nvram_proxy_data_prop = *nvram_proxy_data_prop_ptr;
    const auto proxyData = NVRAM::extractProxyData(nvram_proxy_data_prop.data());
    const auto hasVerbose = proxyData.system_hasBootArg("-v");
    printf("boot-args has -v flag: %d\n", hasVerbose);
    if (!hasVerbose) {
        printf("didn't detect verbose boot flag, bailing out of xnu load\n");
        return;
    }

    auto memory_map_node_ptr = chosen_node.childNamed("memory-map");
    if (!memory_map_node_ptr) {
        printf("couldn't find chosen/memory-map, bailint out of xnu load\n");
        return;
    }
    auto &memory_map_node = *memory_map_node_ptr;
    for (const auto &map_region : memory_map_node.properties_sized(sizeof(DTRegister))) {
        FMT_PRINT("map_region[\"{:s}\"]: {}\n", map_region.name(), map_region.as_reg());
    }

    auto mach_header_prop_ptr = memory_map_node.propertyNamed("Kernel-mach_header");
    if (!mach_header_prop_ptr) {
        printf("couldn't find chosen/memory-map/Kernel-mach_header, bailing out of xnu load\n");
        return;
    }
    auto &mach_header_prop = *mach_header_prop_ptr;
    auto &mach_header_reg = mach_header_prop.as_reg();
    printf("mach_header_reg: addr: %p size: 0x%zx\n", mach_header_reg.base, mach_header_reg.size);
    const auto machoBase = (uintptr_t)mach_header_reg.base;
    printf("machoBase: %p\n", (const void *)machoBase);
    const auto machoBaseOff = machoBase - physBase;
    printf("machoBaseOff: %p\n", (const void *)machoBaseOff);

    auto payload_prop_ptr = memory_map_node.propertyNamed("Kernel-PYLD");
    if (!payload_prop_ptr) {
        printf("couldn't find chosen/memory-map/Kernel-PYLD, bailing out of xnu load\n");
        return;
    }
    auto &payload_prop = *payload_prop_ptr;
    auto &payload_reg = payload_prop.as_reg();
    printf("payload_reg: addr: %p size: 0x%zx\n", payload_reg.base, payload_reg.size);

    const auto &kc = *(const MachO *)payload_reg.base;
    if (!kc.isMagicGood()) {
        printf("bad macho magic: 0x%08x, bailing out of xnu load\n", kc.magic);
        return;
    }
    const auto kc_vmaddr_range = kc.vmaddr_range();
    printf("kcm_vmaddr_range: min: %p max: %p\n", (void *)kc_vmaddr_range.min,
           (void *)kc_vmaddr_range.max);
    const auto kc_physaddr_range = kc_vmaddr_range - virtOff;
    printf("kc_physaddr_range: min: %p max: %p\n", (void *)kc_physaddr_range.min,
           (void *)kc_physaddr_range.max);
    const auto kc_file_range = kc.file_range();
    printf("kc_file_range: min: %p max: %p\n", (void *)kc_file_range.min,
           (void *)kc_file_range.max);
    if (kc_file_range.min != 0) {
        printf("kc_fileoff_range.min != 0 not handled, bailing out of xnu load\n");
        return;
    }
    if (kc_file_range.size() != kc_vmaddr_range.size()) {
        printf("kc vm range != file range not handled, bailing out of xnu load\n");
        return;
    }

    const auto unix_thread_ptr = kc.unixThread();
    if (!unix_thread_ptr) {
        printf("missing unix thread LC, bailing out of xnu load\n");
        return;
    }
    const auto &unix_thread = *unix_thread_ptr;
    const auto entry_pc_vmaddr = unix_thread.pc;
    const auto entry_off = entry_pc_vmaddr - kc_vmaddr_range.min;
    const auto entry_pc = machoBase + entry_off;
    printf("entry_pc: %p\n", (void *)entry_pc);

    auto sepfw_prop_ptr = memory_map_node.propertyNamed("SEPFW");
    if (!sepfw_prop_ptr) {
        printf("couldn't find chosen/memory-map/SEPFW, bailing out of xnu load\n");
        return;
    }
    auto &sepfw_prop = *sepfw_prop_ptr;
    auto &sepfw_reg = sepfw_prop.as_reg();
    printf("sepfw_reg: addr: %p size: 0x%zx\n", sepfw_reg.base, sepfw_reg.size);

    const auto kc_end = machoBase + kc_vmaddr_range.size();
    const auto sepfw_copy_base = kc_end;
    const auto sepfw_copy_end = sepfw_copy_base + sepfw_reg.size;
    printf("sepfw_copy_base: %p\n", (void *)sepfw_copy_base);
    memcpy((char *)sepfw_copy_base, sepfw_reg.base, sepfw_reg.size);

    auto ba_prop_ptr = memory_map_node.propertyNamed("BootArgs");
    if (!ba_prop_ptr) {
        printf("couldn't find chosen/memory-map/BootArgs, bailing out of xnu load\n");
        return;
    }
    auto &ba_prop = *ba_prop_ptr;
    auto &ba_reg = ba_prop.as_reg();
    printf("ba_reg: addr: %p size: 0x%zx\n", ba_reg.base, ba_reg.size);

    const auto ba_copy_base = sepfw_copy_end;
    const auto ba_copy_end = ba_copy_base + ba_reg.size;
    printf("ba_copy_base: %p\n", (void *)ba_copy_base);
    memcpy((char *)ba_copy_base, ba_reg.base, ba_reg.size);

    const auto stub_copy_base = ba_copy_end;
#if M1N1
    memcpy((char *)stub_copy_base, xnu_jump_stub_ptr, stub_size);
#endif
    m1n1::flush_i_and_d_cache((void *)stub_copy_base, stub_size);

    for (const auto &map_region : memory_map_node.properties_sized(sizeof(DTRegister))) {
        FMT_PRINT("map_region[\"{:s}\"]: {}\n", map_region.name(), map_region.as_reg());
    }

    for (auto i = 99;
         const auto &orig_kern_region : memory_map_node.propertiesNamedWithPrefix("Kernel-")) {
        if (orig_kern_region.name() == "Kernel-mach_header"sv) {
            continue;
        }
        FMT_PRINT("orig_kern_region[\"{:s}\"]: {}\n", orig_kern_region.name(),
                  orig_kern_region.as_reg());
        auto &mod_kern_region = (DTProp &)orig_kern_region;
        char new_name_buf[sizeof(mod_kern_region.name_buf)] = {};
        snprintf(new_name_buf, sizeof(new_name_buf), "MemoryMapReserved-%d", i);
        mod_kern_region.setName(new_name_buf);
        (DTRegister &)mod_kern_region.as_reg() = {};
        --i;
    }

    auto mmr_range = memory_map_node.propertiesNamedWithPrefix("MemoryMapReserved-");
    for (auto orig_kern_region = mmr_range.begin(); const auto &seg : kc.segments()) {
        printf("seg: name: %s\n", seg.name().data());
        auto &mod_kern_region = (DTProp &)*orig_kern_region;
        char new_name_buf[sizeof(mod_kern_region.name_buf)] = {};
        snprintf(new_name_buf, sizeof(new_name_buf), "Kernel-%s", seg.name().data());
        mod_kern_region.setName(new_name_buf);
        const auto seg_vm_range = seg.vmaddr_range();
        const auto seg_phys_range = seg_vm_range - virtOff;
        const auto seg_file_range = seg.file_range();
        assert(seg_vm_range.size() == seg_file_range.size());
        FMT_PRINT("seg_vm_range: {}\nseg_phys_range: {}\n", seg_vm_range, seg_phys_range);
        FMT_PRINT("seg_file_range: {}\n", seg_file_range);
        (DTRegister &)mod_kern_region.as_reg() = {(const void *)(machoBase + seg_file_range.min),
                                                  seg_file_range.size()};
        ++orig_kern_region;
    }

    // FIXME: wrong, points to original copy location, not the location it will be copied to further
    // by the stub
    (DTRegister &)sepfw_reg = {(const void *)sepfw_copy_base, sepfw_reg.size};
    (DTRegister &)ba_reg = {(const void *)ba_copy_base, ba_reg.size};

    for (const auto &mod_map_region : memory_map_node.properties_sized(sizeof(DTRegister))) {
        FMT_PRINT("mod_map_region[\"{:s}\"]: {}\n", mod_map_region.name(), mod_map_region.as_reg());
    }

    if (auto iuou_prop_ptr = chosen_node.propertyNamed("internal-use-only-unit")) {
        auto &iuou_prop = *iuou_prop_ptr;
        printf("iuou prop before: 0x%08x\n", iuou_prop.as_u32());
        assert(iuou_prop.size_raw() == 4);
        (uint32_t &)iuou_prop.as_u32() = 1;
        printf("iuou prop after: 0x%08x\n", iuou_prop.as_u32());
    } else {
        printf("internal-use-only-unit prop not found in device tree\n");
        // sad but not fatal
        // return;
    }

    const auto cpuImplRegAddrs = DT::getCPUImplRegAddrs(dt);
    const auto rvbar = entry_pc & ~0xfff;
    for (std::size_t i = 1, e = cpuImplRegAddrs.size(); i < e; ++i) {
        printf("cpuImplRegAddr[%zu]: *%p = %p\n", i, (void *)cpuImplRegAddrs[i], (void *)rvbar);
#if M1N1
        *(uint64_t *)cpuImplRegAddrs[i] = rvbar;
#endif
    }

#if M1N1

#endif
}

} // namespace XNUBoot
