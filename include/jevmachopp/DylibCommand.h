#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Version.h"
#include <string>

class DylibCommand : public LoadSubCommand {
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

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(DylibCommand, struct dylib_command, struct load_command);
#endif
static_assert_size_is(DylibCommand, 16);

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
