#pragma once

#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"

class LoadCommand {
public:
    const LoadSubCommand* subcmd() const;

public:
    LoadCommandType cmd;
    uint32_t cmdsize;
};

static_assert_size_same(LoadCommand, struct load_command);

template <> struct fmt::formatter<LoadCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadCommand const& lc, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "<LoadCommand @ {:p} {}>", (void*)this, lc.cmd);
    }
};