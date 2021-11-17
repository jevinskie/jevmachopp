#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include <mach-o/loader.h>
#include <string>

class DySymtabCommand : public LoadSubCommand {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t ilocalsym;
    uint32_t nlocalsym;
    uint32_t iextdefsym;
    uint32_t nextdefsym;
    uint32_t iundefsym;
    uint32_t nundefsym;
    uint32_t tocoff;
    uint32_t ntoc;
    uint32_t modtaboff;
    uint32_t nmodtab;
    uint32_t extrefsymoff;
    uint32_t nextrefsyms;
    uint32_t indirectsymoff;
    uint32_t nindirectsyms;
    uint32_t extreloff;
    uint32_t nextrel;
    uint32_t locreloff;
    uint32_t nlocrel;

public:
    DySymtabCommand(const DySymtabCommand &) = delete;
    void operator=(const DySymtabCommand &) = delete;
};

static_assert_size_same_minus_header(DySymtabCommand, struct dysymtab_command, struct load_command);

template <> struct fmt::formatter<DySymtabCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(DySymtabCommand const &dySymtabCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return dySymtabCmd.format_to(out);
    }
};
