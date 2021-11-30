#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <span>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Section.h"

using jev_vm_prot_t = int;

class SegmentCommand : public LoadSubCommand {
public:
    SegmentCommand(const SegmentCommand &) = delete;
    void operator=(const SegmentCommand &) = delete;

public:
    std::string_view name() const;
    const Section *sect_cbegin() const;
    const Section *sect_cend() const;
    uint32_t sect_size() const;
    std::size_t sect_sizeof() const;
    std::span<const Section> sections() const;
    AddrRange vmaddr_range() const;
    AddrRange file_range() const;

public:
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    jev_vm_prot_t maxprot;
    jev_vm_prot_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(SegmentCommand, struct segment_command_64,
                                     struct load_command);
#endif
static_assert_size_is(SegmentCommand, 64);

#if USE_FMT
template <> struct fmt::formatter<SegmentCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(SegmentCommand const &segCmd, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<SegmentCommand @ {:p} \"{:s}\">"_cf, (void *)segCmd.loadCommand(),
                       segCmd.name());
        return out;
    }
};
#endif
