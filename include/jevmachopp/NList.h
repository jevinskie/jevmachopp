#pragma once

#include "jevmachopp/Common.h"

#include <mach-o/nlist.h>
#include <string>

class NList {
public:
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t strx;
    uint8_t type;
    uint8_t sect;
    uint16_t desc;
    uint64_t value;

public:
    NList(const NList &) = delete;
    void operator=(const NList &) = delete;
};

static_assert_size_same(NList, struct nlist_64);

template <> struct fmt::formatter<NList> {

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(NList const &nlist, format_context &ctx) {
        auto out = ctx.out();
        return nlist.format_to(out);
    }
};
