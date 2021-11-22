#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class LinkeditDataCommand : public LoadSubCommand {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint32_t name_offset;
    uint32_t timestamp;
    uint32_t current_version;
    uint32_t compatibility_version;

public:
    LinkeditDataCommand(const LinkeditDataCommand &) = delete;
    void operator=(const LinkeditDataCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(LinkeditDataCommand, struct dylib_command,
                                     struct load_command);
#endif
static_assert_size_is(LinkeditDataCommand, 16);

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
