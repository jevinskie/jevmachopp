#include "jevmachopp/Section.h"

std::string Section::sectName() const {
    return readMaybeNullTermCString<decltype(sectname)>(sectname);
}

std::string Section::segName() const {
    return readMaybeNullTermCString<decltype(segname)>(segname);
}

fmt::appender &Section::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<Section @ {:p} {:s},{:s}>"_cf, (void *)this, segName(), sectName());
    return out;
}
