#include "jevmachopp/CallFinder.h"
#include "jevmachopp/CallStubs.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/Section.h"
#include "jevmachopp/SegmentCommand.h"

namespace CallFinder {

static constexpr std::uint32_t BYTES_PER_STUB = 16;

bool findCallsTo(const MachO &macho, const std::string_view symbol_name) {
    const SymtabCommand *symtab = macho.symtab();
    assert(symtab);
    const auto dylib_name_map = macho.dylibNamesMap();
    const auto seg_name_map = macho.segmentNamesMap();
    const NList *symbol_nl_ptr = find_if_or_nullptr(macho.indirect_syms(), [&](const NList &nl) {
        return nl.name(macho, *symtab) == symbol_name && nl.segmentName(seg_name_map) == "__TEXT"sv;
    });
    if (!symbol_nl_ptr) {
        return false;
    }
    FMT_PRINT("symbol_nl: {:m}\n", *symbol_nl_ptr, macho, fmt::ptr(&dylib_name_map));

    const auto symbol_indir_idx = get_index_in(macho.indirect_syms(), [&](const NList &nl) {
        return nl.name(macho, *symtab) == symbol_name && nl.segmentName(seg_name_map) == "__TEXT"sv;
    });
    if (!symbol_indir_idx) {
        return false;
    }
    printf("symbol_indir_idx: %zu\n", *symbol_indir_idx);

    const SegmentCommand *text_seg = macho.textSeg();
    assert(text_seg);

    const Section *stub_sect = nullptr;
    setIfNull(stub_sect, [=]() {
        return text_seg->sectionNamed("__auth_stubs");
    });
    setIfNull(stub_sect, [=]() {
        return text_seg->sectionNamed("__stubs");
    });
    assert(stub_sect);

    const auto stubs_base = stub_sect->addr;
    const auto symbol_stub_addr = stubs_base + *symbol_indir_idx * BYTES_PER_STUB;
    printf("symbol_stub_addr: %p\n", (void *)symbol_stub_addr);

    return true;
}

} // namespace CallFinder
