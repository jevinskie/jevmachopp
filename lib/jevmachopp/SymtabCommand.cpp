#include "jevmachopp/SymtabCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

const NList *SymtabCommand::nlists_cbegin(const MachO &macho) const {
    return (const NList *)((uintptr_t)&macho + symoff);
}

const NList *SymtabCommand::nlists_cend(const MachO &macho) const {
    return (const NList *)((uintptr_t)nlists_cbegin(macho) + nlists_sizeof());
}

std::size_t SymtabCommand::nlists_size() const {
    return nsyms;
}

std::size_t SymtabCommand::nlists_sizeof() const {
    return nlists_size() * sizeof(NList);
}

std::span<const NList> SymtabCommand::nlists(const MachO &macho) const {
    return {nlists_cbegin(macho), nlists_cend(macho)};
}

PackedCStrIterator SymtabCommand::strtab_cbegin(const MachO &macho) const {
    return PackedCStrIterator(strtab_data(macho));
}

PackedCStrIterator SymtabCommand::strtab_cend(const MachO &macho) const {
    return PackedCStrIterator(strtab_data(macho) + strtab_sizeof());
}

std::size_t SymtabCommand::strtab_sizeof() const {
    return strsize;
}

const char *SymtabCommand::strtab_data(const MachO &macho) const {
    return (const char *)((uintptr_t)&macho + stroff);
}

packed_cstr_entry_range SymtabCommand::strtab_entries(const MachO &macho) const {
    return {strtab_cbegin(macho), strtab_cend(macho)};
}

#if USE_FMT
fmt::appender &SymtabCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<SymtabCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
#endif
