#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

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
};

ranges::filter_view<MachO::lc_range, MachO::lc_pred> MachO::segmentLoadCommands() const {
    return ranges::views::filter(
        loadCommands(),
        (lc_pred)[](const LoadCommand &lc) { return lc.cmd == LoadCommandType::SEGMENT_64; });
};
// ranges::any_view<const LoadCommand &> MachO::segments() const {
//     return ranges::views::transform(segmentLoadCommands(), [](auto &&segLC) {
//         return std::get<const SegmentCommand *>(segLC.subcmd()->get());
//     });
// };
const SegmentCommand *MachO::segmentWithName(const std::string &name) {
    return nullptr;
};

fmt::appender &MachO::format_to(fmt::appender &out) const {
    fmt::format_to(
        out,
        "<MachO @ {:p} cputype: {} fileType: {:#010x} flags: {:#010x} ncmds: {:d} sizeofcmds: {:#x} "_cf,
        (void *)this, cputype, filetype, flags, ncmds, sizeofcmds);
    fmt::format_to(out, "{}"_cf, fmt::join(loadCommands(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}