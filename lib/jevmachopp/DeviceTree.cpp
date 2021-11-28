#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/Hex.h"
#include "jevmachopp/NVRAM.h"
#include "jevmachopp/c/jevdtree.h"

#include <cstring>
#include <numeric>

#include <nanorange/views/drop.hpp>

#pragma mark DTRegister

#if USE_FMT
fmt::appender &DTRegister::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTRegister @ base: {:p} size: {:#x}"_cf, base, size);
    return out;
}
#endif

#pragma mark DTProp

#pragma mark DTProp - Accessors

const char *DTProp::name() const {
    assert(std::memchr(name_buf, '\0', sizeof(name_buf)));
    return name_buf;
}

void DTProp::setName(const char *newName) {
    assert(strlen(newName) < sizeof(name_buf));
    memset(name_buf, '\0', sizeof(name_buf));
    strcpy(name_buf, newName);
}

std::uint32_t DTProp::size_raw() const {
    return sz & ~(PROP_SIZE_REPLACEMENT_TAG);
}

std::uint32_t DTProp::size_padded() const {
    return (size_raw() + 3) & ~(3u);
}

const uint8_t *DTProp::data() const {
    return (const uint8_t *)(this + 1);
}

const char *DTProp::as_cstr() const {
    return (const char *)data();
}

const uint32_t &DTProp::as_u32() const {
    assert(size_raw() == sizeof(uint32_t));
    return *(const uint32_t *)data();
}

const uint64_t &DTProp::as_u64() const {
    assert(size_raw() == sizeof(uint64_t));
    return *(const uint64_t *)data();
}

const DTRegister &DTProp::as_reg() const {
    assert(size_raw() == sizeof(DTRegister));
    return *(const DTRegister *)data();
}

bool DTProp::isReplacement() const {
    return (sz & PROP_SIZE_REPLACEMENT_TAG) != 0;
}

#pragma mark DTProp - fmt

#if USE_FMT
fmt::appender &DTProp::format_to(fmt::appender &out) const {
    auto hexstr = buf2hexstr<65536>(data(), size_raw());
    fmt::format_to(out,
                   "<DTProp @ {:p} name: \"{:s}\" size raw: {:d} size padded: {:d} data: {:s}>"_cf,
                   (void *)this, name(), size_raw(), size_padded(), hexstr);
    return out;
}
#endif

#pragma mark DTNode

#pragma mark DTNode - properties

DTProp::prop_range DTNode::properties() const {
    return {properties_cbegin(), properties_cend()};
}

DTProp::Iterator DTNode::properties_cbegin() const {
    return DTProp::Iterator{(const DTProp *)(this + 1), properties_size()};
}

DTProp::Iterator DTNode::properties_cend() const {
    return DTProp::Iterator{};
}

std::uint32_t DTNode::properties_size() const {
    return nprops;
}

std::uint32_t DTNode::properties_sizeof() const {
    return std::accumulate(properties_cbegin(), properties_cend(), 0,
                           [](const auto &accum, const auto &prop) {
                               return accum + sizeof(prop) + prop.size_padded();
                           });
}

const DTProp *DTNode::propertyNamed(const std::string_view &name) const {
    return find_if_or_nullptr(properties(), [=](const DTProp &prop) {
        return prop.name() == name;
    });
}

#pragma mark DTNode - children

DTNode::child_range DTNode::children() const {
    return {children_cbegin(), children_cend()};
}

DTNode::Iterator DTNode::children_cbegin() const {
    return DTNode::Iterator{(const DTNode *)((uintptr_t)(this + 1) + properties_sizeof()),
                            children_size()};
}

DTNode::Iterator DTNode::children_cend() const {
    return DTNode::Iterator{};
}

std::uint32_t DTNode::children_size() const {
    return nchildren;
}

std::uint32_t DTNode::children_sizeof() const {
    return std::accumulate(children_cbegin(), children_cend(), 0,
                           [](const auto &accum, const auto &child) {
                               return accum + child.node_sizeof();
                           });
}

const DTNode *DTNode::childNamed(const std::string_view name) const {
    return find_if_or_nullptr(children(), [=](const DTNode &child) {
        const auto *name_ptr = child.name_or_nullptr();
        if (!name_ptr) {
            return false;
        }
        return name_ptr == name;
    });
}

#pragma mark DTNode - Traversal

const DTNode *DTNode::lookupNode(std::string_view nodePath) const {
    printf("DTNode::lookupNode(\"%.*s\")\n", sv2pf(nodePath).sz, sv2pf(nodePath).str);
    if (!nodePath.size() || nodePath[0] != '/') {
        return nullptr;
    }
    auto *res = this;
    auto childrenNamesRng = stringSplitViewDelimitedBy(nodePath, '/') | views::drop(1);
    for (auto i = childrenNamesRng.begin(), e = childrenNamesRng.end(); i != e; ++i) {
        res = childNamed(*i);
        if (!res) {
            return nullptr;
        }
    }
    return res;
}

const DTProp *DTNode::lookupProperty(std::string_view propertyPath) const {
    printf("DTNode::lookupProperty(\"%.*s\")\n", sv2pf(propertyPath).sz, sv2pf(propertyPath).str);
    return nullptr;
}

#pragma mark DTNode - Accessors

const char *DTNode::name_or_nullptr() const {
    const DTProp *name_prop = propertyNamed("name");
    return name_prop ? (const char *)name_prop->data() : nullptr;
}

const char *DTNode::name() const {
    const char *name_ptr = name_or_nullptr();
    assert(name_ptr);
    return name_ptr;
}

std::uint32_t DTNode::node_sizeof() const {
    return sizeof(*this) + properties_sizeof() + children_sizeof();
}

#pragma mark DTNode - fmt

#if USE_FMT
fmt::appender &DTNode::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTNode @ {:p} name: \"{:s}\" # props: {:d} # children: {:d} props: "_cf,
                   (void *)this, name(), nprops, nchildren);
    fmt::format_to(out, "{}"_cf, fmt::join(properties(), ", "));
    fmt::format_to(out, " children: "_cf);
    fmt::format_to(out, "{}"_cf, fmt::join(children(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}
#endif

#pragma mark namaespace DT

#pragma mark namaespace DT - Utilities

std::experimental::fixed_capacity_vector<const uint64_t *, DT::MAX_CPUS>
DT::getCPUImplRegAddrs(const DTNode &rootNode) {
    std::experimental::fixed_capacity_vector<const uint64_t *, DT::MAX_CPUS> regs{};
    if (auto cpus_node_ptr = rootNode.childNamed("cpus")) {
        auto &cpus_node = *cpus_node_ptr;
        for (const auto &cpu_node : cpus_node.children()) {
            if (auto cpu_impl_reg_prop_ptr = cpu_node.propertyNamed("cpu-impl-reg")) {
                const auto &cpu_impl_reg_prop = *cpu_impl_reg_prop_ptr;
                regs.emplace_back(*(const uint64_t **)cpu_impl_reg_prop.data());
            } else {
                printf("chosen/cpus/cpuX/cpu-impl-reg DT prop missing\n");
            }
        }
    } else {
        printf("chosen/cpus DT node missing\n");
    }
    return regs;
}

#pragma mark C

void dump_dtree(const void *dtree_buf) {
    printf("dtree @ %p\n", dtree_buf);

    FMT_PRINT("dtree: {:p}\n", dtree_buf);

    auto dtree_root_node_ptr = (const DTNode *)dtree_buf;
    auto &dtree_root_node = *dtree_root_node_ptr;
    // FMT_PRINT("dtree_root_node: {}\n", dtree_root_node);

    printf("DT root node # props: %u # children: %u\n", dtree_root_node.properties_size(),
           dtree_root_node.children_size());

    // for (const auto &prop : dtree_root_node.properties()) {
    //     printf("DT root node prop: name: %s\n", prop.name());
    // }

    auto chosen_node_ptr = dtree_root_node.childNamed("chosen");
    if (chosen_node_ptr) {
        auto &chosen_node = *chosen_node_ptr;
        // FMT_PRINT("chosen_node: {}\n", chosen_node);

        auto iuou_prop_ptr = chosen_node.propertyNamed("internal-use-only-unit");
        if (iuou_prop_ptr) {
            auto &iuou_prop = *iuou_prop_ptr;
            FMT_PRINT("iuou prop: {}\n", iuou_prop);
            printf("iuou prop before: 0x%08x\n", iuou_prop.as_u32());
            assert(iuou_prop.size_raw() == 4);
            (uint32_t &)iuou_prop.as_u32() = 1;
            FMT_PRINT("iuou prop: {}\n", iuou_prop);
            printf("iuou prop after: 0x%08x\n", iuou_prop.as_u32());
        } else {
            printf("internal-use-only-unit prop not found in device tree\n");
        }

        auto nvram_proxy_data_prop_ptr = chosen_node.propertyNamed("nvram-proxy-data");
        if (nvram_proxy_data_prop_ptr) {
            auto &nvram_proxy_data_prop = *nvram_proxy_data_prop_ptr;
            // FMT_PRINT("nvram-proxy-data prop: {}\n", nvram_proxy_data_prop);
            const auto proxyData = NVRAM::extractProxyData(nvram_proxy_data_prop.data());
            const char *bootArgsVarEqValStr = proxyData.system_part.varNamed("boot-args");
            if (bootArgsVarEqValStr) {
                const char *bootArgs = NVRAM::varValue(bootArgsVarEqValStr);
                FMT_PRINT("nvram boot-args: {:s}\n", bootArgs);
                printf("nvram boot-args: %s\n", bootArgs);
                for (const auto &bootArg : rangeForSpaceDelimitedCStr(bootArgs)) {
                    FMT_PRINT("boot-arg: {:s}\n", bootArg);
                    printf("boot-arg: %.*s\n", (int)bootArg.size(), bootArg.data());
                }
                const auto hasVerbose = proxyData.system_hasBootArg("-v");
                FMT_PRINT("boot-args has -v flag: {}\n", hasVerbose);
                printf("boot-args has -v flag: %d\n", hasVerbose);
            } else {
                printf("boot-args missing\n");
            }
        }
    } else {
        printf("chosen node not found in device tree\n");
    }

    auto cpus_node_ptr = dtree_root_node.childNamed("cpus");
    if (cpus_node_ptr) {
        auto &cpus_node = *cpus_node_ptr;
        for (const auto &cpu_node : cpus_node.children()) {
            // FMT_PRINT("cpu_node: {}", cpu_node);
            auto cpu_impl_reg_prop_ptr = cpu_node.propertyNamed("cpu-impl-reg");
            if (cpu_impl_reg_prop_ptr) {
                const auto &cpu_impl_reg_prop = *cpu_impl_reg_prop_ptr;
                FMT_PRINT("cpu_impl_reg_prop: {}\n", cpu_impl_reg_prop);
            } else {
                printf("chosen/cpus/cpuX/cpu-impl-reg DT prop missing\n");
            }
        }
    } else {
        printf("chosen/cpus DT node missing\n");
    }

    const auto cpuImplRegAddrs = DT::getCPUImplRegAddrs(dtree_root_node);
    for (const auto &cpuImplRegAddr : cpuImplRegAddrs) {
        printf("cpuImplRegAddr: %p\n", (const void *)cpuImplRegAddr);
    }
}
