#include "jevmachopp/MachO.h"
#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/SymtabCommand.h"

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

const SegmentCommand *MachO::segmentWithName(const std::string &name) const {
    return ranges::find_if_or_nullptr(segments(), [=](const SegmentCommand &segCmd) {
        return segCmd.segName() == name;
    });
}

const SymtabCommand *MachO::symtab() const {
    return (const SymtabCommand *)ranges::find_if_or_nullptr(
        loadCommands(), [=](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::SYMTAB;
        });
}

const DySymtabCommand *MachO::dysymtab() const {
    return (const DySymtabCommand *)ranges::find_if_or_nullptr(
        loadCommands(), [=](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::DYSYMTAB;
        });
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
