#pragma once

#include <cstdint>
#include <vector>

#include "fmt/core.h"

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UnknownCommand : public LoadSubCommand {
public:
    fmt::appender &format_to(fmt::appender &out) const;
};

static_assert_size_is(UnknownCommand, 1);

template <> struct fmt::formatter<UnknownCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(UnknownCommand const &unkCmd, FormatContext &ctx) {
        auto out = ctx.out();
        unkCmd.format_to(out);
        return out;
    }
};
