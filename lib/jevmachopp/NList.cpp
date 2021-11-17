#include "jevmachopp/NList.h"

fmt::appender &NList::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<NList @ {:p}>"_cf, (void *)this);
    return out;
}
