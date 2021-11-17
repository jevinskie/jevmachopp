#pragma once

#include "jevmachopp/Common.h"

#include <mach-o/nlist.h>
#include <string>

class NList {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t symoff; // nlist array offset from <FIXME WHERE?>
    uint32_t nsyms;
    uint32_t stroff;  // strtab offset from <FIXME WHERE?>
    uint32_t strsize; // bytes

public:
    NList(const NList &) = delete;
    void operator=(const NList &) = delete;
};

static_assert_size_same(NList, struct nlist_64);

template <> struct fmt::formatter<NList> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(NList const &nlist, FormatContext &ctx) {
        auto out = ctx.out();
        return nlist.format_to(out);
    }
};
