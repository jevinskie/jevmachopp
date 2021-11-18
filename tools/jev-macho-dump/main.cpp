#include <assert.h>
#include <fmt/core.h>
#include <jevmachopp/MachO.h>
#include <jevmachopp/NList.h>
#include <jevmachopp/SegmentCommand.h>
#include <jevmachopp/Slurp.h>
#include <jevmachopp/SymtabCommand.h>

int main(int argc, const char *argv[]) {
    assert(argc == 2);
    std::string infile(argv[1]);
    uint64_t size;
    uint8_t *inbuf = Slurp::readfile(infile, &size);
    auto macho_ptr = (const MachO *)inbuf;
    auto &macho = *macho_ptr;

    fmt::print("macho: {}\n", (void *)&macho);
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

    return 0;
}
