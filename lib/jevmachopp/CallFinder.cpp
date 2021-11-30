#include "jevmachopp/CallFinder.h"
#include "jevmachopp/MachO.h"

namespace CallFinder {

bool findCallsTo(const MachO &macho, const std::string_view symbol_name) {
    const SymtabCommand *symtab = macho.symtab();
    assert(symtab);
    const auto seg_name_map = macho.segmentNamesMap();
    const NList *symbol_nl_ptr = find_if_or_nullptr(macho.indirect_syms(), [&](const NList &nl) {
        return nl.name(macho, *symtab) == symbol_name && nl.segmentName(seg_name_map) == "__TEXT"sv;
    });
    if (symbol_nl_ptr) {
        FMT_PRINT("symbol_nl: {:f}\n", *symbol_nl_ptr, macho);
    }
    int idx = 0;
    for (const auto &nl : macho.indirect_syms()) {
        // FMT_PRINT("indirect_sym[{:3d}]: {:f}\n", idx++, nl, macho);
        printf("indirect_sym[%3d]: \"%s\"\n", idx++, nl.name(macho, *symtab));
    }
    return false;
}

} // namespace CallFinder
