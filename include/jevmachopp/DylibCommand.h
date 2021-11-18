#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include <mach-o/loader.h>
#include <string>

class DylibCommand : public LoadSubCommand {
public:
    class Version {
    public:
        Version(uint32_t ver);
        uint16_t major;
        uint8_t minor;
        uint8_t patch;
    };

public:
    const char *name() const;
    bool isID() const;
    bool isLoad() const;
    bool isLoadWeak() const;
    bool isRexport() const;
    Version currentVersion() const;
    Version compatibilityVersion() const;
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t name_offset;
    uint32_t timestamp;
    uint32_t current_version;
    uint32_t compatibility_version;

public:
    DylibCommand(const DylibCommand &) = delete;
    void operator=(const DylibCommand &) = delete;
};

static_assert_size_same_minus_header(DylibCommand, struct dylib_command, struct load_command);

template <> struct fmt::formatter<DylibCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(DylibCommand const &dylibCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return dylibCmd.format_to(out);
    }
};

template <> struct fmt::formatter<DylibCommand::Version> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    auto format(DylibCommand::Version const &ver, format_context &ctx) -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{:d}.{:d}.{:d}", ver.major, ver.minor, ver.patch);
    }
};
