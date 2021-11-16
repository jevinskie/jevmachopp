#pragma once

#include <list>
#include <mach/machine.h>
#include <memory>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/subrange.hpp>
#include <stdint.h>
#include <string>
#include <typeinfo>

#include "fmt/core.h"
#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"

class LoadCommand;

class MachO {
public:
    MachO(const MachO &) = delete;
    void operator=(const MachO &) = delete;

public:
    ranges::subrange<LoadCommand::Iterator> loadCommands() const {
        return {lc_cbegin(), lc_cend()};
    };
    LoadCommand::Iterator lc_cbegin() const;
    LoadCommand::Iterator lc_cend() const;
    size_t lc_size() const;
    size_t lc_sizeof() const;

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
        fmt::format_to(
            out,
            "<MachO @ {:p} cputype: {} fileType: {:#010x} flags: {:#010x} ncmds: {:d} sizeofcmds: {:#x}>"_cf,
            (void *)&macho, macho.cputype, macho.filetype, macho.flags, macho.ncmds,
            macho.sizeofcmds);

        // for (auto i = macho.lc_cbegin(), e = macho.lc_cend(); i != e; i = std::next(i)) {
        //     fmt::format_to(out, "\ni: {}", (void *)&*i);
        //     fmt::format_to(out, "\nlc: {}", *i);
        //     auto subcmd = i->subcmd();
        // }

        for (auto &&lc : macho.loadCommands()) {
            fmt::format_to(out, "\ni: {}", (void *)&lc);
            fmt::format_to(out, "\nlc: {}", lc);
            auto subcmd = lc.subcmd();
        }

        return out;
    }
};
