#include "jevmachopp/NVRAM.h"
#include "jevmachopp/c/jevnvram.h"

#pragma mark C

void dump_nvram(const void *nvram_buf) {
    printf("nvram @ %p\n", nvram_buf);

#if USE_FMT

    fmt::print("nvram: {:p}\n", nvram_buf);

#endif
}
