#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/c/jevdtree.h"

#pragma mark DTProp

#pragma mark DTProp - Accessors

const char *DTProp::name() const {
    assert(memchr(name_buf, '\0', sizeof(name_buf)));
    return name_buf;
}

std::uint32_t DTProp::size_raw() const {
    return sz & ~(0x80000000u);
}

std::uint32_t DTProp::size_padded() const {
    return (size_raw() + 3) & ~(3u);
}

#pragma mark DTProp - fmt

fmt::appender &DTProp::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTProp @ {:p} name: \"{:s}\" size raw: {:d} size padded: {:d}>"_cf,
                   (void *)this, name(), size_raw(), size_padded());
    return out;
}

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

#pragma mark DTNode - fmt

fmt::appender &DTNode::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTNode @ {:p} # props: {:d} # children: {:d} props: "_cf, (void *)this,
                   nprops, nchildren);
    fmt::format_to(out, "{}"_cf, fmt::join(properties(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}

#pragma mark C

void dump_dtree(const void *dtree_buf) {
    printf("dtree @ %p\n", dtree_buf);

#if USE_FMT

    fmt::print("dtree: {:p}\n", dtree_buf);

    auto dtree_root_node_ptr = (const DTNode *)dtree_buf;
    auto &dtree_root_node = *dtree_root_node_ptr;
    fmt::print("dtree_root_node: {}\n", dtree_root_node);

#endif
}
