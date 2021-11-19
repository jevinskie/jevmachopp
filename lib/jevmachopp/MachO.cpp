#include "jevmachopp/MachO.h"
#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Strtab.h"
#include "jevmachopp/SymtabCommand.h"

#pragma mark MachO header

bool MachO::isMagicGood() const {
    return magic == MH_MAGIC_64;
}

#pragma mark load commands

LoadCommand::Iterator MachO::lc_cbegin() const {
    return LoadCommand::Iterator((const LoadCommand *)(this + 1));
}

LoadCommand::Iterator MachO::lc_cend() const {
    return LoadCommand::Iterator((const LoadCommand *)((uintptr_t)(this + 1) + lc_sizeof()));
}

size_t MachO::lc_size() const {
    return ncmds;
}

size_t MachO::lc_sizeof() const {
    return sizeofcmds;
}

MachO::lc_range MachO::loadCommands() const {
    return {lc_cbegin(), lc_cend()};
}

#pragma mark segments

ranges::any_view<const LoadCommand &> MachO::segmentLoadCommands() const {
    return ranges::views::filter(loadCommands(), [](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::SEGMENT_64;
    });
}

ranges::any_view<const SegmentCommand &> MachO::segments() const {
    return ranges::views::transform(
        segmentLoadCommands(), [](const LoadCommand &segLC) -> const SegmentCommand & {
            return *std::get<const SegmentCommand *>(segLC.subcmd()->get());
        });
}

const SegmentCommand *MachO::segmentWithName(const std::string_view &name) const {
    return ranges::find_if_or_nullptr(segments(), [=](const SegmentCommand &segCmd) {
        return segCmd.segName() == name;
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
    const auto *lc = ranges::find_if_or_nullptr(loadCommands(), [=](const LoadCommand &lc) {
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

ranges::any_view<const char *> MachO::symtab_strtab_entries() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return {};
    }
    return ranges::views::transform(symtab_ptr->strtab_entries(*this),
                                    [](const auto &strchr) -> const char * {
                                        return &strchr;
                                    });
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
    const auto *lc = ranges::find_if_or_nullptr(loadCommands(), [=](const LoadCommand &lc) {
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

size_t MachO::local_syms_size() const {
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

size_t MachO::ext_def_syms_size() const {
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

size_t MachO::undef_syms_size() const {
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

#define MKMEM(memFknPtr, object)                      \
    (delegate<std::remove_reference_t<memFknPtr>>::make<std::remove_reference_t<decltype(object)>, \
                               memFknPtr>(object))

ranges::any_view<const NList &> MachO::indirect_syms(const SymtabCommand *symtab_ptr,
                                                     const DySymtabCommand *dysymtab_ptr) const {
    // least rescanning, but ugly
    // symtab_ptr = set_if_null(symtab_ptr, symtab);
    // symtab_ptr = set_if_null_w_checked_ret(symtab_ptr, symtab, {});

    auto del = delegate<const SymtabCommand *()>::make<&MachO::symtab>(*this);
    if (!setIfNull(symtab_ptr, del)) {
        return {};
    }
    // setIfNullErroringRet(symtab_ptr, delegate<const SymtabCommand *()>::make<&MachO::symtab>(*this),
    //                      {});
    setIfNullErroringRet(dysymtab_ptr,
                         delegate<const DySymtabCommand *()>::make<&MachO::dysymtab>(*this), {});
    const auto nlists = symtab_ptr->nlists(*this);
    return ranges::views::transform(indirect_syms_idxes(),
                                    [nlists](const int idx) -> const NList & {
                                        return nlists[idx];
                                    });
}

size_t MachO::indirect_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nindirectsyms;
}

fmt::appender &MachO::format_to(fmt::appender &out) const {
    fmt::format_to(
        out,
        "<MachO @ {:p} cputype: {} fileType: {:#010x} flags: {:#010x} ncmds: {:d} sizeofcmds: {:#x} "_cf,
        (void *)this, cputype, filetype, flags, ncmds, sizeofcmds);
    fmt::format_to(out, "{}"_cf, fmt::join(loadCommands(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}
