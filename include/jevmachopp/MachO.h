#pragma once

#include <list>
#include <mach/machine.h>
#include <memory>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/Packer.h"

class MachO : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    CpuTypeMeta cputype;
    uint32_t filetype;
    uint32_t flags;
    std::list<LoadCommand> loadCommands;

  private:
    uint64_t loadCommandSize() const;
};

template <> struct fmt::formatter<MachO> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(MachO const &macho, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<MachO cputype: {} fileType: {:#010x} flags: {:#010x}>",
                              macho.cputype, macho.filetype, macho.flags);
    }
};
