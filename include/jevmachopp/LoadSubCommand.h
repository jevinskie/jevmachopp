#pragma once

#include "jevmachopp/Common.h"

#include <variant>

class LoadCommand;
class BuildVersionCommand;
class DylibCommand;
class DySymtabCommand;
class EncryptionInfoCommand;
class FilesetEntryCommand;
class LinkeditDataCommand;
class SegmentCommand;
class SourceVersionCommand;
class SymtabCommand;
class UUIDCommand;
class UnixThreadCommand;
class UnknownCommand;

using SubCommandVariant =
    std::variant<const BuildVersionCommand *, const DylibCommand *, const DySymtabCommand *,
                 const EncryptionInfoCommand *, const FilesetEntryCommand *,
                 const LinkeditDataCommand *, const SegmentCommand *, const SourceVersionCommand *,
                 const SymtabCommand *, const UUIDCommand *, const UnixThreadCommand *,
                 const UnknownCommand *>;

class LoadSubCommand {
public:
    LoadSubCommand(const LoadSubCommand &) = delete;
    void operator=(const LoadSubCommand &) = delete;

public:
    const LoadCommand *loadCommand() const;
    const SubCommandVariant get() const;
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif
};

static_assert_size_is(LoadSubCommand, 1);

#if USE_FMT
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
#endif