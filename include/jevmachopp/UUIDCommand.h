#pragma once

#include <stdint.h>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UUIDCommand : public LoadSubCommand {
public:
    UUIDCommand(const UUIDCommand &) = delete;
    void operator=(const UUIDCommand &) = delete;

public:
    uint8_t uuid[16];
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(UUIDCommand, struct uuid_command, struct load_command);
#endif
static_assert_size_is(UUIDCommand, 16);

#if USE_FMT
template <> struct fmt::formatter<UUIDCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(UUIDCommand const &uuid, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<UUID @ {:p} ", (void *)this);
        const uint8_t *p = uuid.uuid;
        for (int i = 0; i < 4; ++i) {
            fmt::format_to(out, "{:02x}", *p);
            ++p;
        }
        fmt::format_to(out, "-");
        for (int i = 0; i < 2; ++i) {
            fmt::format_to(out, "{:02x}", *p);
            ++p;
        }
        fmt::format_to(out, "-");
        for (int i = 0; i < 2; ++i) {
            fmt::format_to(out, "{:02x}", *p);
            ++p;
        }
        fmt::format_to(out, "-");
        for (int i = 0; i < 2; ++i) {
            fmt::format_to(out, "{:02x}", *p);
            ++p;
        }
        fmt::format_to(out, "-");
        for (int i = 0; i < 6; ++i) {
            fmt::format_to(out, "{:02x}", *p);
            ++p;
        }
        return out;
    }
};
#endif
