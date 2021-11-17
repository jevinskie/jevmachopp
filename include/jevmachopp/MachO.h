#pragma once

#include <list>
#include <mach/machine.h>
#include <memory>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/subrange.hpp>
#include <stdint.h>
#include <string>
#include <typeinfo>

#include "fmt/core.h"
#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"

class SegmentCommand;

class MachO {
public:
    MachO(const MachO &) = delete;
    void operator=(const MachO &) = delete;

public:
    using lc_range = ranges::subrange<LoadCommand::Iterator>;
    lc_range loadCommands() const;
    ranges::any_view<const LoadCommand &> segmentLoadCommands() const;
    ranges::any_view<const SegmentCommand &> segments() const;
    const SegmentCommand *segmentWithName(const std::string &name);
    LoadCommand::Iterator lc_cbegin() const;
    LoadCommand::Iterator lc_cend() const;
    size_t lc_size() const;
    size_t lc_sizeof() const;
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t magic;
    CpuTypeMeta cputype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};

static_assert_size_same(MachO, struct mach_header_64);

template <> struct fmt::formatter<MachO> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(MachO const &macho, FormatContext &ctx) {
        auto out = ctx.out();
        return macho.format_to(out);
    }
};
