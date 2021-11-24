#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/Hex.h"
#include "jevmachopp/c/jevdtree.h"

#include <cstring>
#include <numeric>

#pragma mark DTProp

#pragma mark DTProp - Accessors

const char *DTProp::name() const {
    assert(std::memchr(name_buf, '\0', sizeof(name_buf)));
    return name_buf;
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

bool DTProp::isReplacement() const {
    return (sz & PROP_SIZE_REPLACEMENT_TAG) != 0;
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

const DTNode *DTNode::childNamed(const std::string_view &name) const {
    return find_if_or_nullptr(children(), [=](const DTNode &child) {
        const auto *name_ptr = child.name_or_nullptr();
        if (!name_ptr) {
            return false;
        }
        return name_ptr == name;
    });
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

fmt::appender &DTNode::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTNode @ {:p} name: \"{:s}\" # props: {:d} # children: {:d} props: "_cf,
                   (void *)this, name(), nprops, nchildren);
    fmt::format_to(out, "{}"_cf, fmt::join(properties(), ", "));
    fmt::format_to(out, " children: "_cf);
    fmt::format_to(out, "{}"_cf, fmt::join(children(), ", "));
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
    // fmt::print("dtree_root_node: {}\n", dtree_root_node);

    auto chosen_node_ptr = dtree_root_node.childNamed("chosen");
    if (chosen_node_ptr) {
        auto &chosen_node = *chosen_node_ptr;
        fmt::print("chosen_node: {}\n", chosen_node);

        auto iuou_prop_ptr = chosen_node.propertyNamed("internal-use-only-unit");
        if (iuou_prop_ptr) {
            auto &iuou_prop = *iuou_prop_ptr;
            fmt::print("iuou prop: {}\n", iuou_prop);
            assert(iuou_prop.size_raw() == 4);
            auto *iuou_prop_buf_rw = (uint8_t *)iuou_prop.data();
            auto hexstr = buf2hexstr(iuou_prop_buf_rw, iuou_prop.size_raw());
            fmt::print("iuou prop: hexstr {:s}\n", hexstr);
            iuou_prop_buf_rw[0] = 1; // little endian... right?
            fmt::print("iuou prop: {}\n", iuou_prop);
        }
    }

#endif
}
