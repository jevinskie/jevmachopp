#include "jevmachopp/CallFinder.h"
#include "jevmachopp/ARM64Disasm.h"
#include "jevmachopp/CallStubs.h"
#include "jevmachopp/FunctionStartsCommand.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/Section.h"
#include "jevmachopp/SegmentCommand.h"

#include <concepts>

namespace CallFinder {

static constexpr std::uint32_t BYTES_PER_STUB = 16;

bool findCallsTo(const MachO &macho, const std::string_view symbol_name) {
    FMT_PRINT("type: {}\n", type_name<nano::iterator_t<raw_func_start_range>>());

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
    assert(text_seg->vmaddr_range().size() < 128 * 1024 * 1024); // BL branch distance

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

    printf("__TEXT filesize: 0x%llx\n", text_seg->filesize);

    const auto &text_raw = text_seg->data(macho);
    const auto &text_instr = span_cast<const uint32_t>(text_raw);

    uint64_t pc = text_seg->vmaddr_range().min;
    for (const auto instr_raw : text_instr) {
        if (ARM64Disasm::isBLTo(instr_raw, pc, symbol_stub_addr)) {
            printf("found BL @ %p to %.*s\n", (void *)pc, sv2pf(symbol_name).sz,
                   sv2pf(symbol_name).str);
        }
        pc += 4;
    }

    const auto func_starts_cmd = macho.functionStartsCommand();
    assert(func_starts_cmd);

    const auto func_starts_raw = func_starts_cmd->raw_offsets(macho);
    for (const uint64_t fsro : func_starts_raw) {
        printf("fsro: %p\n", (void *)fsro);
    }

    auto func_starts_file_offs = func_starts_cmd->file_offsets(macho, text_seg);
    for (const uint64_t fsfo : func_starts_file_offs) {
        printf(">> fsfo: %p\n", (void *)fsfo);
    }
    for (const uint64_t fsfo : func_starts_file_offs) {
        printf(">2 fsfo: %p\n", (void *)fsfo);
    }
    // auto b = func_starts_file_offs.begin();
    // FMT_PRINT("b type: {:s}\n", type_name<decltype(b)>());

    // FMT_PRINT("fart: {:s}\n", type_name<common_reference_t<LEB128Iterator<uint64_t>>>());
    // // FMT_PRINT("b: {}\n", b);
    // printf("b: %llx\n", static_cast<uint64_t>(*b));
    // auto db = *b;

    // const auto func_starts_vmaddrs = func_starts_cmd->vm_addrs();
    // for (const uint64_t fsvm : func_starts_file_offs) {
    //     printf("## fsvm: %p\n", (void *)fsfo);
    // }

    return true;
}

} // namespace CallFinder
