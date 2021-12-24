#include "jevmachopp/FatMachO.h"

#if USE_FMT
fmt::appender &FatMachO::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<FatMachO>"_cf);
    return out;
}
#endif
