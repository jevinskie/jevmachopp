#include "jevmachopp/NList.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SymtabCommand.h"

using namespace fmt::literals;

const char *NList::name(const char *strtab_data) const {
    return strtab_data + strx;
}

const char *NList::name(const MachO &macho, const SymtabCommand &symtabCmd) const {
    return name(symtabCmd.strtab_data(macho));
}

const char *NList::name(const MachO &macho) const {
    return name(macho.strtab_data());
}

uint8_t NList::dylibOrdinal() const {
    assert((type & JEV_N_TYPE) == JEV_N_UNDF && sect == JEV_NO_SECT);
    return ((desc) >> 8) & 0xff;
}

const char *NList::dylibName(const dylib_names_map_t &map) const {
    return map[dylibOrdinal() - 1];
}

#if USE_FMT
fmt::appender &NList::format_to(fmt::appender &out) const {
    fmt::format_to(
        out, "<NList @ {:p} strx: {:#x} type: {:#04x} sect: {:d} desc: {:#06x} value: {:#018x}>"_cf,
        (void *)this, strx, type, sect, desc, value);
    return out;
}

fmt::appender &NList::format_to(fmt::appender &out, const MachO &macho,
                                const dylib_names_map_t *map) const {
    assert(macho.isMagicGood());
    fmt::format_to(
        out,
        "<NList @ {:p} strx: {:#x} type: {:#04x} sect: {:d} desc: {:#06x} value: {:#018x} name: \"{:s}\""_cf,
        (void *)this, strx, type, sect, desc, value, name(macho));
    if (map) {
        fmt::format_to(out, " dylib: \"{:s}\""_cf, dylibName(*map));
    }
    fmt::format_to(out, ">"_cf);
    return out;
}
#endif
