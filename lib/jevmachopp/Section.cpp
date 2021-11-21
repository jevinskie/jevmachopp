#include "jevmachopp/Section.h"

fmt::appender &Section::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<Section @ {:p} {:s},{:s}>"_cf, (void *)this, segName(), sectName());
    return out;
}
