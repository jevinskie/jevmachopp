#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Version.h"

#include <mach-o/loader.h>

class SourceVersionCommand : public LoadSubCommand {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    SourceVersion version; // A.B.C.D.E

public:
    SourceVersionCommand(const SourceVersionCommand &) = delete;
    void operator=(const SourceVersionCommand &) = delete;
};

static_assert_size_same_minus_header(SourceVersionCommand, struct source_version_command,
                                     struct load_command);

template <> struct fmt::formatter<SourceVersionCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(SourceVersionCommand const &sourceVerCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return sourceVerCmd.format_to(out);
    }
};
