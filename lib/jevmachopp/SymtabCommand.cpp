#include "jevmachopp/SymtabCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

const NList *SymtabCommand::nlists_cbegin() const {
    return (const NList *)(this + 1);
}

const NList *SymtabCommand::nlists_cend() const {
    return (const NList *)((uintptr_t)nlists_cbegin() + nlists_sizeof());
}

size_t SymtabCommand::nlists_size() const {
    return nsyms;
}

size_t SymtabCommand::nlists_sizeof() const {
    return nlists_size() * sizeof(NList);
}

SymtabCommand::nlist_range SymtabCommand::nlists() const {
    return {nlists_cbegin(), nlists_cend()};
}

fmt::appender &SymtabCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<SymtabCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
