#include "jevmachopp/NList.h"

using namespace fmt::literals;

fmt::appender &NList::format_to(fmt::appender &out) const {
    fmt::format_to(
        out, "<NList @ {:p} strx: {:#x} type: {:#04x} sect: {:d} desc: {:#06x} value: {:#018x}>"_cf,
        (void *)this, strx, type, sect, desc, value);
    return out;
}

fmt::appender &NList::format_to(fmt::appender &out, const MachO &macho,
                                const dylib_names_map_t *map) const {
    auto name = macho.strtab_data() + strx;
    fmt::format_to(
        out,
        "<NList @ {:p} strx: {:#x} type: {:#04x} sect: {:d} desc: {:#06x} value: {:#018x} name: \"{:s}\""_cf,
        (void *)this, strx, type, sect, desc, value, name);
    if (map && (type & N_TYPE) == N_UNDF && sect == NO_SECT) {
        fmt::format_to(out, " dylib: \"{:s}\""_cf, (*map)[GET_LIBRARY_ORDINAL(desc) - 1]);
    }
    fmt::format_to(out, ">"_cf);
    return out;
}
