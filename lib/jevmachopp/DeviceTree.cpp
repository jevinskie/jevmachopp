#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/c/jevdtree.h"

#pragma mark DTNode

#pragma mark DTNode - fmt

fmt::appender &DTNode::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTNode @ {:p} # props: {:d} # children: {:d}>"_cf, (void *)this, nprops,
                   nchildren);
    return out;
}

#pragma mark DTProp

#pragma mark DTProp - fmt

fmt::appender &DTProp::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTProp @ {:p} name: \"{:s}\" size: {:d}>"_cf, (void *)this, name(), sz);
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
