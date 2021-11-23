#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/c/jevdtree.h"

#pragma mark DTreeNode

#pragma mark DTreeNode - fmt

fmt::appender &DTreeNode::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DTreeNode @ {:p} # props: {:d} # children: {:d}>"_cf, (void *)this,
                   nprops, nchildren);
    return out;
}

#pragma mark C

void dump_dtree(const void *dtree_buf) {
    printf("dtree @ %p\n", dtree_buf);

#if USE_FMT

    fmt::print("dtree: {:p}\n", dtree_buf);

#endif
}
