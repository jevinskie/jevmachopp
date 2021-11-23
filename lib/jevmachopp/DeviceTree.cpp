#include "jevmachopp/DeviceTree.h"
#include "jevmachopp/c/jevdtree.h"

#pragma mark C

void dump_dtree(const void *dtree_buf) {
    printf("dtree @ %p\n", dtree_buf);
}
