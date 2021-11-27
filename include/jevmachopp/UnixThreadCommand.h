#pragma once

#include <stdint.h>

#include <fmt/core.h>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UnixThreadCommand : public LoadSubCommand {
public:
    UnixThreadCommand(const UnixThreadCommand &) = delete;
    void operator=(const UnixThreadCommand &) = delete;

public:
    uint32_t flavor; // Flavor of thread state
    uint32_t count;  // Count of uint32_t's in thread state
    uint64_t x[29];  // General purpose registers x0-x28
    uint64_t fp;     // Frame pointer x29
    uint64_t lr;     // Link register x30
    uint64_t sp;     // Stack pointer x31
    uint64_t pc;     // Program counter
    uint32_t cpsr;   // Current program status register
    uint32_t flags;  // Flags describing structure format
};

static_assert_size_is(UnixThreadCommand, 280);

#if USE_FMT
template <> struct fmt::formatter<UnixThreadCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(UnixThreadCommand const &thread, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<UnixThreadCommand pc: {:p}>", (void *)thread.pc);

        return out;
    }
};
#endif
