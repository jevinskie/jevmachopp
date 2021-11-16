#pragma once

#include <list>
#include <mach-o/loader.h>
#include <mach/vm_prot.h>
#include <memory>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Section.h"

class SegmentCommand : public LoadSubCommand {
public:
    SegmentCommand(const SegmentCommand &) = delete;
    void operator=(const SegmentCommand &) = delete;

public:
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    vm_prot_t maxprot;
    vm_prot_t initprot;
    uint32_t nsects;
    uint32_t flags;

public:
    std::string segName() const;
};

static_assert_size_same_minus_header(SegmentCommand, struct segment_command_64,
                                     struct load_command);

template <> struct fmt::formatter<SegmentCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(SegmentCommand const &segCmd, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<SegmentCommand @ {:p} \"{:s}\">"_cf, (void *)segCmd.loadCommand(),
                       segCmd.segName());
        return out;
    }
};
