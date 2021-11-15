#pragma once

#include <mach/machine.h>
#include <stdint.h>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/subrange.hpp>
#include <list>
#include <memory>
#include <string>

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"
#include "fmt/core.h"

class LoadCommand;

class MachO {
  public:
    ranges::subrange<const LoadCommand*> loadCommands() const;

  public:
    uint32_t  magic;
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
        return fmt::format_to(ctx.out(), "<MachO cputype: {} fileType: {:#010x} flags: {:#010x}>",
                              macho.cputype, macho.filetype, macho.flags);
    }
};
