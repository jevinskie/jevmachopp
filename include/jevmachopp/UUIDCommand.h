#pragma once

#include <mach-o/loader.h>
#include <stdint.h>

#include <fmt/core.h>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UUIDCommand : public LoadSubCommand {
  public:
    uint8_t uuid[16];
};

static_assert_size_same_minus_header(UUIDCommand, struct uuid_command, struct load_command);


template <> struct fmt::formatter<UUIDCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(UUIDCommand const &uuid, FormatContext &ctx) {
        return fmt::format_to(
            ctx.out(),
            "<UUID @ {:p} "
            "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}>",
            (void*)this,
            uuid.uuid[0], uuid.uuid[1], uuid.uuid[2], uuid.uuid[3],
            uuid.uuid[4], uuid.uuid[5],
            uuid.uuid[6], uuid.uuid[7],
            uuid.uuid[8], uuid.uuid[9],
            uuid.uuid[10], uuid.uuid[11], uuid.uuid[12], uuid.uuid[13], uuid.uuid[14], uuid.uuid[15]);
    }
};
