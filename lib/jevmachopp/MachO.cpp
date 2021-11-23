#include <algorithm>
#include <cstddef>

#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/FilesetEntryCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Strtab.h"
#include "jevmachopp/c/jevmacho.h"

#pragma mark MachO header

bool MachO::isMagicGood() const {
    return magic == JEV_MH_MAGIC_64;
}

#pragma mark load commands

LoadCommand::Iterator MachO::lc_cbegin() const {
    return LoadCommand::Iterator((const LoadCommand *)(this + 1));
}

LoadCommand::Iterator MachO::lc_cend() const {
    return LoadCommand::Iterator((const LoadCommand *)((uintptr_t)(this + 1) + lc_sizeof()));
}

std::size_t MachO::lc_size() const {
    return ncmds;
}

std::size_t MachO::lc_sizeof() const {
    return sizeofcmds;
}

MachO::lc_range MachO::loadCommands() const {
    return {lc_cbegin(), lc_cend()};
}

#pragma mark segments

const SegmentCommand *MachO::segmentWithName(const std::string_view &name) const {
    return find_if_or_nullptr(segments(), [=](const SegmentCommand &segCmd) {
        return segCmd.segName().data() == name;
    });
}

const SegmentCommand *MachO::textSeg() const {
    return segmentWithName("__TEXT");
}

const SegmentCommand *MachO::dataConstSeg() const {
    return segmentWithName("__DATA_CONST");
}

const SegmentCommand *MachO::dataSeg() const {
    return segmentWithName("__DATA");
}

const SegmentCommand *MachO::linkeditSeg() const {
    return segmentWithName("__LINKEDIT");
}

#pragma mark symtab

const SymtabCommand *MachO::symtab() const {
    const auto *lc = find_if_or_nullptr(loadCommands(), [](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::SYMTAB;
    });
    if (!lc) {
        return nullptr;
    }
    return (const SymtabCommand *)lc->subcmd();
}

std::span<const NList> MachO::symtab_nlists() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return {};
    }
    return symtab_ptr->nlists(*this);
}

const char *MachO::strtab_data() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return nullptr;
    }
    return symtab_ptr->strtab_data(*this);
}

#pragma mark dysymtab

const DySymtabCommand *MachO::dysymtab() const {
    const auto *lc = find_if_or_nullptr(loadCommands(), [=](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::DYSYMTAB;
    });
    if (!lc) {
        return nullptr;
    }
    return (const DySymtabCommand *)lc->subcmd();
}

std::span<const NList> MachO::local_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->ilocalsym],
            &nlists[dysymtab_ptr->ilocalsym + dysymtab_ptr->nlocalsym]};
}

std::size_t MachO::local_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nlocalsym;
}

std::span<const NList> MachO::ext_def_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->iextdefsym],
            &nlists[dysymtab_ptr->iextdefsym + dysymtab_ptr->nextdefsym]};
}

std::size_t MachO::ext_def_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nextdefsym;
}

std::span<const NList> MachO::undef_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->iundefsym],
            &nlists[dysymtab_ptr->iundefsym + dysymtab_ptr->nundefsym]};
}

std::size_t MachO::undef_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nundefsym;
}

std::span<const uint32_t> MachO::indirect_syms_idxes(const DySymtabCommand *dysymtab_ptr) const {
    if (!dysymtab_ptr) {
        dysymtab_ptr = dysymtab();
        if (!dysymtab_ptr) {
            return {};
        }
    }
    return {(const uint32_t *)((uintptr_t)this + dysymtab_ptr->indirectsymoff),
            dysymtab_ptr->nindirectsyms};
}

std::size_t MachO::indirect_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nindirectsyms;
}

#pragma mark dylibs

auto MachO::dylibNamesMap() const -> decltype(dylibNamesMap()) {
    dylib_names_map_t res = {};
    ranges::copy(ranges::views::transform(importedDylibCommands(),
                                          [](const DylibCommand &dylibCmd) -> const char * {
                                              return dylibCmd.name();
                                          }),
                 res.data());
    return res;
}

#pragma mark fmt

#if USE_FMT
fmt::appender &MachO::format_to(fmt::appender &out) const {
    fmt::format_to(
        out, "<MachO @ {:p} cputype: {} fileType: {} flags: {} ncmds: {:d} sizeofcmds: {:#x} "_cf,
        (void *)this, cputype, filetype, flags, ncmds, sizeofcmds);
    fmt::format_to(out, "{}"_cf, fmt::join(loadCommands(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}
#endif

#pragma mark C

void dump_macho(const void *macho_header) {
    int idx;
    auto macho_ptr = (const MachO *)macho_header;
    auto &macho = *macho_ptr;

    FMT_PRINT("macho: {}\n", (void *)&macho);
    printf("macho: %p\n", &macho);

    FMT_PRINT("macho fmt: {}\n", macho);
    FMT_PRINT("macho->cputype: {}\n", macho.cputype);

#if 0
    for (const auto [idx, lc] : ranges::views::enumerate(macho.loadCommands())) {
        FMT_PRINT("lc[{:2d}]: {}\n", idx, lc);
    }

    for (const auto &segLC : macho.segmentLoadCommands()) {
        FMT_PRINT("segment load cmd: {}\n", segLC);
    }

    FMT_PRINT("ret: {:s}\n", type_name<decltype(macho.segments())>());

    for (const auto &segCmd : macho.segments()) {
        FMT_PRINT("segment cmd: {}\n", segCmd);
        for (const auto &sect : segCmd.sections()) {
            FMT_PRINT("section: {}\n", sect);
        }
    }

    const SegmentCommand *textSeg = macho.segmentWithName("__TEXT");
    if (!textSeg) {
        FMT_PRINT("textSeg: nullptr\n");
    } else {
        FMT_PRINT("textSeg: {}\n", *textSeg);
    }
#endif

#if 0
    const SymtabCommand *symtab_ptr = macho.symtab();
    if (!symtab_ptr) {
        FMT_PRINT("symtab: nullptr\n");
    } else {
        FMT_PRINT("symtab: {}\n", *symtab_ptr);
    }
    const SymtabCommand &symtab = *symtab_ptr;

    for (const auto [idx, nl] : ranges::views::enumerate(macho.symtab_nlists())) {
        FMT_PRINT("nlist[{:3d}]: {:f}\n", idx, nl, macho);
    }

    for (const auto [idx, ste] : ranges::views::enumerate(macho.symtab_strtab_entries())) {
        FMT_PRINT("strtab[{:3d}]: {:s}\n", idx, ste);
    }
#endif

#if 0
    for (const auto [idx, nl] : ranges::views::enumerate(macho.local_syms())) {
        FMT_PRINT("local_sym[{:3d}]: {:f}\n", idx, nl, macho);
    }

    for (const auto [idx, nl] : ranges::views::enumerate(macho.ext_def_syms())) {
        FMT_PRINT("ext_def_sym[{:3d}]: {:f}\n", idx, nl, macho);
    }
#endif

    // for (const auto [idx, lc] : ranges::views::enumerate(macho.loadCommands())) {
    //     FMT_PRINT("lc[{:2d}]: {}\n", idx, lc);
    // }

    idx = 0;
    for (const auto &lc : macho.loadCommands()) {
        // FMT_PRINT("lc[{:2d}]: {}\n", idx++, lc);
        // printf("lc[%2d]: %s\n", idx++,
        // LoadCommandType_traits::to_string_or_empty(lc.cmd).data());
        printf("lc[%2d]: @ %p type: 0x%08x\n", idx++, &lc, as_unsigned(lc.cmd));
    }

    const SymtabCommand *symtab_ptr = macho.symtab();
    assert(symtab_ptr);
    const SymtabCommand &symtab = *symtab_ptr;
    const SegmentCommand *linkedit_seg_ptr = macho.linkeditSeg();
    assert(linkedit_seg_ptr);
    const SegmentCommand &linkedit_seg = *linkedit_seg_ptr;

    // FMT_PRINT("linkedit_seg: {}\n", linkedit_seg);
    printf("linkedit_seg: %p\n", &linkedit_seg);

    const auto dylib_names = macho.dylibNamesMap();

    // for (const auto [idx, nl] : ranges::views::enumerate(macho.undef_syms())) {
    //     FMT_PRINT("undef_sym[{:3d}]: {:m}\n", idx, nl, macho, (const void *)&dylib_names);
    // }

    idx = 0;
    for (const auto &nl : macho.undef_syms()) {
        // FMT_PRINT("undef_sym[{:3d}]: {:m}\n", idx++, nl, macho, (const void *)&dylib_names);
        printf("undef_sym[%3d]: \"%s\" from \"%s\"\n", idx++, nl.name(macho, symtab),
               nl.dylibName(dylib_names));
    }

    // for (const auto [idx, nl] : ranges::views::enumerate(macho.indirect_syms())) {
    //     FMT_PRINT("indirect_sym[{:3d}]: {:f}\n", idx, nl, macho);
    // }

    idx = 0;
    for (const auto &nl : macho.indirect_syms()) {
        // FMT_PRINT("indirect_sym[{:3d}]: {:f}\n", idx++, nl, macho);
        printf("indirect_sym[%3d]: \"%s\"\n", idx++, nl.name(macho, symtab));
    }

    idx = 0;
    for (const auto &fseCmd : macho.filesetEntryCommands()) {
        // FMT_PRINT("indirect_sym[{:3d}]: {:f}\n", idx++, nl, macho);
        printf("fse[%3d]: vmaddr: 0x%016lx fileoff: 0x%08lx name: \"%s\"\n", idx++, fseCmd.vmaddr,
               fseCmd.fileoff, fseCmd.name());
    }
}
