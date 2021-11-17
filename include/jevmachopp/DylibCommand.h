#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class DylibCommand : public LoadSubCommand {
public:
    DylibCommand(const DylibCommand &) = delete;
    void operator=(const DylibCommand &) = delete;

public:
    uint32_t dummy[4];
};

static_assert_size_same_minus_header(DylibCommand, struct dylib_command, struct load_command);

template <> struct fmt::formatter<DylibCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(DylibCommand const &dylibCmd, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<DylibCommand @ {:p} \"{:s}\">"_cf, (void *)dylibCmd.loadCommand(),
                       "todo.dylib");
        return out;
    }
};
