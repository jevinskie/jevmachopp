#include <assert.h>
#include <fmt/core.h>
#include <jevmachopp/FilesetEntryCommand.h>
#include <jevmachopp/MachO.h>
#include <jevmachopp/NList.h>
#include <jevmachopp/SegmentCommand.h>
#include <jevmachopp/Slurp.h>
#include <jevmachopp/SymtabCommand.h>

int main(int argc, const char *argv[]) {
    int idx;
    assert(argc == 2);
    uint64_t size;
    uint8_t *inbuf = Slurp::readfile(argv[1], &size);
    auto macho_ptr = (const MachO *)inbuf;
    auto &macho = *macho_ptr;

    // fmt::print("macho: {}\n", (void *)&macho);
    printf("macho: %p\n", &macho);

#if 0
    // fmt::print("macho fmt: {}\n", macho);
    fmt::print("macho->cputype: {}\n", macho.cputype);

    for (const auto [idx, lc] : ranges::views::enumerate(macho.loadCommands())) {
        fmt::print("lc[{:2d}]: {}\n", idx, lc);
    }

    for (const auto &segLC : macho.segmentLoadCommands()) {
        fmt::print("segment load cmd: {}\n", segLC);
    }

    fmt::print("ret: {:s}\n", type_name<decltype(macho.segments())>());

    for (const auto &segCmd : macho.segments()) {
        fmt::print("segment cmd: {}\n", segCmd);
        for (const auto &sect : segCmd.sections()) {
            fmt::print("section: {}\n", sect);
        }
    }

    const SegmentCommand *textSeg = macho.segmentWithName("__TEXT");
    if (!textSeg) {
        fmt::print("textSeg: nullptr\n");
    } else {
        fmt::print("textSeg: {}\n", *textSeg);
    }
#endif

#if 0
    const SymtabCommand *symtab_ptr = macho.symtab();
    if (!symtab_ptr) {
        fmt::print("symtab: nullptr\n");
    } else {
        fmt::print("symtab: {}\n", *symtab_ptr);
    }
    const SymtabCommand &symtab = *symtab_ptr;

    for (const auto [idx, nl] : ranges::views::enumerate(macho.symtab_nlists())) {
        fmt::print("nlist[{:3d}]: {:f}\n", idx, nl, macho);
    }

    for (const auto [idx, ste] : ranges::views::enumerate(macho.symtab_strtab_entries())) {
        fmt::print("strtab[{:3d}]: {:s}\n", idx, ste);
    }
#endif

#if 0
    for (const auto [idx, nl] : ranges::views::enumerate(macho.local_syms())) {
        fmt::print("local_sym[{:3d}]: {:f}\n", idx, nl, macho);
    }

    for (const auto [idx, nl] : ranges::views::enumerate(macho.ext_def_syms())) {
        fmt::print("ext_def_sym[{:3d}]: {:f}\n", idx, nl, macho);
    }
#endif

    // for (const auto [idx, lc] : ranges::views::enumerate(macho.loadCommands())) {
    //     fmt::print("lc[{:2d}]: {}\n", idx, lc);
    // }

    idx = 0;
    for (const auto &lc : macho.loadCommands()) {
        // fmt::print("lc[{:2d}]: {}\n", idx++, lc);
        // printf("lc[%2d]: %s\n", idx++,
        // LoadCommandType_traits::to_string_or_empty(lc.cmd).data());
        printf("lc[%2d]: @ %p type: 0x%08x\n", idx++, &lc, as_unsigned(lc.cmd));
    }

    // const SymtabCommand *symtab_ptr = macho.symtab();
    // assert(symtab_ptr);
    // const SymtabCommand &symtab = *symtab_ptr;
    const SegmentCommand *linkedit_seg_ptr = macho.linkeditSeg();
    assert(linkedit_seg_ptr);
    const SegmentCommand &linkedit_seg = *linkedit_seg_ptr;

    // fmt::print("linkedit_seg: {}\n", linkedit_seg);
    printf("linkedit_seg: %p\n", &linkedit_seg);

    // const auto dylib_names = macho.dylibNamesMap();

    // for (const auto [idx, nl] : ranges::views::enumerate(macho.undef_syms())) {
    //     fmt::print("undef_sym[{:3d}]: {:m}\n", idx, nl, macho, (const void *)&dylib_names);
    // }

    // idx = 0;
    // for (const auto &nl : macho.undef_syms()) {
    //     // fmt::print("undef_sym[{:3d}]: {:m}\n", idx++, nl, macho, (const void *)&dylib_names);
    //     printf("undef_sym[%3d]: \"%s\" from \"%s\"\n", idx++, nl.name(macho, symtab),
    //            nl.dylibName(dylib_names));
    // }

    // for (const auto [idx, nl] : ranges::views::enumerate(macho.indirect_syms())) {
    //     fmt::print("indirect_sym[{:3d}]: {:f}\n", idx, nl, macho);
    // }

    // idx = 0;
    // for (const auto &nl : macho.indirect_syms()) {
    //     // fmt::print("indirect_sym[{:3d}]: {:f}\n", idx++, nl, macho);
    //     printf("indirect_sym[%3d]: \"%s\"\n", idx++, nl.name(macho, symtab));
    // }

    idx = 0;
    for (const auto &fseCmd : macho.filesetEntryCommands()) {
        // fmt::print("indirect_sym[{:3d}]: {:f}\n", idx++, nl, macho);
        printf("fse[%3d]: vmaddr: 0x%016llx fileoff: 0x%08llx name: \"%s\"\n", idx++, fseCmd.vmaddr,
               fseCmd.fileoff, fseCmd.name());
    }

    return 0;
}
