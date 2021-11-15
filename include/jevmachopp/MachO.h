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
    // ranges::subrange<const LoadCommand*> loadCommands() const;
    const LoadCommand *lc_cbegin() const;
    const LoadCommand *lc_cend() const;
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
        out = fmt::format_to(out, "<MachO cputype: {} fileType: {:#010x} flags: {:#010x}>",
                             macho.cputype, macho.filetype, macho.flags);

        for (auto i = macho.lc_cbegin(), e = macho.lc_cend(); i != e; i = std::next(i)) {
            // out = fmt::format_to(out, "\ni: {}", (void*)i);
        }
        return out;
    }
};
