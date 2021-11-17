#include "jevmachopp/NList.h"

fmt::appender &NList::format_to(fmt::appender &out) const {
    fmt::format_to(
        out, "<NList @ {:p} strx: {:#x} type: {:#04x} sect: {:u} desc: {:#06x} value: {:#018x}>",
        (void *)this, strx, type, sect, desc, value);
    return out;
}
