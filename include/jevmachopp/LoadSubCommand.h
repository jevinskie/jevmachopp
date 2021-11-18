#pragma once

#include "jevmachopp/Common.h"

#include "fmt/core.h"
#include <variant>

class LoadCommand;
class DylibCommand;
class DySymtabCommand;
class EncryptionInfoCommand;
class LinkeditDataCommand;
class SegmentCommand;
class SymtabCommand;
class UUIDCommand;
class UnknownCommand;

using SubCommandVariant =
    std::variant<const DylibCommand *, const DySymtabCommand *, const EncryptionInfoCommand *,
                 const LinkeditDataCommand *, const SegmentCommand *, const SymtabCommand *,
                 const UUIDCommand *, const UnknownCommand *>;

class LoadSubCommand {
public:
    LoadSubCommand(const LoadSubCommand &) = delete;
    void operator=(const LoadSubCommand &) = delete;

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
