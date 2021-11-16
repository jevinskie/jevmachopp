#pragma once

#include <mach-o/loader.h>
#include <stdint.h>
#include <vector>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class EncryptionInfoCommand : public LoadSubCommand {
public:
    EncryptionInfoCommand(const EncryptionInfoCommand &) = delete;
    void operator=(const EncryptionInfoCommand &) = delete;

public:
    uint32_t cryptoff;
    uint32_t cryptsize;
    uint32_t cryptid;
    uint32_t pad;
};

static_assert_size_same_minus_header(EncryptionInfoCommand, struct encryption_info_command_64,
                                     struct load_command);

template <> struct fmt::formatter<EncryptionInfoCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(EncryptionInfoCommand const &encInfoCmd, FormatContext &ctx) {
        auto out = ctx.out();
        fmt::format_to(out, "<EncryptionInfoCommand @ {:p}>"_cf, (void *)encInfoCmd.loadCommand());
        return out;
    }
};
