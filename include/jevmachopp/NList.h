#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/MachO.h"

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
    // Presentation format: 'd' - dumb, 'f' - fancy.
    char presentation = 'd';
    int macho_arg_id = -1;

    auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'd' || *it == 'f'))
            presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}')
            throw format_error("invalid format");

        if (presentation == 'f') {
            macho_arg_id = ctx.next_arg_id();
        }

        // Return an iterator past the end of the parsed range:
        return it;
    }

    auto format(NList const &nlist, format_context &ctx) -> decltype(ctx.out()) {
        auto out = ctx.out();
        fmt::print("\nWHATWHAT ctx: {} THATSWHAT\n", presentation);
        if (macho_arg_id >= 0) {
            auto macho_arg = ctx.arg(macho_arg_id);
            const MachO *macho_ptr = nullptr;
            visit_format_arg(
                [](auto v) {
//                    auto v = custom.value;
                    fmt::print("fuck\n");
                },
                macho_arg);
            fmt::print("");
        }
        return nlist.format_to(out);
    }
};
