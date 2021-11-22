#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Version.h"

class BuildVersionCommand : public LoadSubCommand {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t platform;
    Version minos;
    Version sdk;
    uint32_t ntools;

public:
    BuildVersionCommand(const BuildVersionCommand &) = delete;
    void operator=(const BuildVersionCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(BuildVersionCommand, struct build_version_command,
                                     struct load_command);
#endif
static_assert_size_is(BuildVersionCommand, 16);

template <> struct fmt::formatter<BuildVersionCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(BuildVersionCommand const &buildVerCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return buildVerCmd.format_to(out);
    }
};
