#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include <mach-o/loader.h>
#include <string>

class SymtabCommand : public LoadSubCommand {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t symoff; // nlist array offset from <FIXME WHERE?>
    uint32_t nsyms;
    uint32_t stroff;  // strtab offset from <FIXME WHERE?>
    uint32_t strsize; // bytes

public:
    SymtabCommand(const SymtabCommand &) = delete;
    void operator=(const SymtabCommand &) = delete;
};

static_assert_size_same_minus_header(SymtabCommand, struct symtab_command, struct load_command);

template <> struct fmt::formatter<SymtabCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(SymtabCommand const &symtabCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return symtabCmd.format_to(out);
    }
};
