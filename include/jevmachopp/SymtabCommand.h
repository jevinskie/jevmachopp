#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/NList.h"
#include "jevmachopp/PackedCStr.h"

#include <cstddef>
#include <span>

class MachO;

class SymtabCommand : public LoadSubCommand {
public:
    const NList *nlists_cbegin(const MachO &macho) const;
    const NList *nlists_cend(const MachO &macho) const;
    std::size_t nlists_size() const;
    std::size_t nlists_sizeof() const;
    std::span<const NList> nlists(const MachO &macho) const;

    PackedCStrIterator strtab_cbegin(const MachO &macho) const;
    PackedCStrIterator strtab_cend(const MachO &macho) const;
    std::size_t strtab_sizeof() const;
    const char *strtab_data(const MachO &macho) const;
    packed_cstr_range strtab_entries(const MachO &macho) const;
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint32_t symoff; // nlist array offset from MachO header
    uint32_t nsyms;
    uint32_t stroff;  // strtab offset from MachO header
    uint32_t strsize; // bytes

public:
    SymtabCommand(const SymtabCommand &) = delete;
    void operator=(const SymtabCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(SymtabCommand, struct symtab_command, struct load_command);
#endif
static_assert_size_is(SymtabCommand, 16);

#if USE_FMT
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
#endif
