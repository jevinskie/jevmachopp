#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/NList.h"
#include "jevmachopp/Strtab.h"
#include <mach-o/loader.h>

class MachO;

class SymtabCommand : public LoadSubCommand {
public:
    using nlist_range = ranges::subrange<const NList *>;
    const NList *nlists_cbegin(const MachO &macho) const;
    const NList *nlists_cend(const MachO &macho) const;
    size_t nlists_size() const;
    size_t nlists_sizeof() const;
    nlist_range nlists(const MachO &macho) const;

    using strtab_entry_range = ranges::subrange<StrtabIterator>;
    StrtabIterator strtab_cbegin(const MachO &macho) const;
    StrtabIterator strtab_cend(const MachO &macho) const;
    size_t strtab_sizeof() const;
    strtab_entry_range strtab_entries(const MachO &macho) const;

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
