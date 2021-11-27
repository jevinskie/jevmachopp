#include "jevmachopp/CommonTypes.h"

#if USE_FMT
fmt::appender &AddrRange::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<AddrRange min: {:p} max: {:p} size: {:#x}>"_cf, (const void *)min,
                   (const void *)max, size());
    return out;
}
#endif
