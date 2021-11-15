#pragma once

#include <list>
#include <mach/machine.h>
#include <memory>
#include <stdint.h>
#include <string>

#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/subrange.hpp>

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/Packer.h"

//struct mach_header_64 {
//    uint32_t    magic;        /* mach magic number identifier */
//    cpu_type_t    cputype;    /* cpu specifier */
//    cpu_subtype_t    cpusubtype;    /* machine specifier */
//    uint32_t    filetype;    /* type of file */
//    uint32_t    ncmds;        /* number of load commands */
//    uint32_t    sizeofcmds;    /* the size of all the load commands */
//    uint32_t    flags;        /* flags */
//    uint32_t    reserved;    /* reserved */
//};

class MachO : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;
    ranges::subrange<const LoadCommand*> loadCommands() const;


  public:
    CpuTypeMeta cputype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;

  private:
    uint64_t loadCommandSize() const;
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
