#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class MachO;
class SegmentCommand;

class LinkeditDataCommand : public LoadSubCommand {
public:
    std::span<const uint8_t> data(const MachO &macho) const;

public:
    uint32_t dataoff;
    uint32_t datasize;

public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    LinkeditDataCommand(const LinkeditDataCommand &) = delete;
    void operator=(const LinkeditDataCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(LinkeditDataCommand, struct linkedit_data_command,
                                     struct load_command);
#endif
static_assert_size_is(LinkeditDataCommand, 8);

#if USE_FMT
template <> struct fmt::formatter<LinkeditDataCommand> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    auto format(LinkeditDataCommand const &ledLC, format_context &ctx) -> decltype(ctx.out()) {
        auto out = ctx.out();
        return ledLC.format_to(out);
    }
};
#endif
