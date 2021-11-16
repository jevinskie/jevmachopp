#pragma once

#include "jevmachopp/Common.h"

#include "fmt/core.h"
#include <variant>

class LoadCommand;
class EncryptionInfoCommand;
class SegmentCommand;
class UUIDCommand;
class UnknownCommand;

using SubCommandVariant = std::variant<const EncryptionInfoCommand *, const SegmentCommand *,
                                       const UUIDCommand *, const UnknownCommand *>;

class LoadSubCommand {
public:
    const LoadCommand *loadCommand() const;
    const SubCommandVariant get() const;
    fmt::appender &format_to(fmt::appender &out) const;
};

static_assert_size_is(LoadSubCommand, 1);

template <> struct fmt::formatter<LoadSubCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadSubCommand const &lsc, FormatContext &ctx) {
        auto out = ctx.out();
        lsc.format_to(out);
        return out;
    }
};
