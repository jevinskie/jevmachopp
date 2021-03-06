#pragma once

#include <cstdint>
#include <vector>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UnknownCommand : public LoadSubCommand {
public:
    UnknownCommand(const UnknownCommand &) = delete;
    void operator=(const UnknownCommand &) = delete;

public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif
};

static_assert_size_is(UnknownCommand, 1);

#if USE_FMT
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
#endif
