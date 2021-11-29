#include "jevmachopp/Section.h"

std::string_view Section::sectName() const {
    return readMaybeNullTermCString(sectname, sizeof(sectname));
}
std::string_view Section::segName() const {
    return readMaybeNullTermCString(segname, sizeof(segname));
}

#if USE_FMT
fmt::appender &Section::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<Section @ {:p} {:s},{:s}>"_cf, (void *)this, segName(), sectName());
    return out;
}
#endif
