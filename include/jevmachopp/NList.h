#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/MachO.h"

#include <cassert>
#include <mach-o/nlist.h>
#include <string>

class NList {
public:
    fmt::appender &format_to(fmt::appender &out) const;
    fmt::appender &format_to(fmt::appender &out, const MachO &macho) const;

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

template <typename T> struct value_extractor {
    T operator()(T value) {
        return value;
    }

    template <typename U> [[noreturn]] T operator()(U) {
        assert(!"invalid type");
    }
};

template <> struct fmt::formatter<NList> {
    // Presentation format: 'd' - dumb, 'f' - fancy.
    char presentation = 'd';
    int macho_arg_id = -1;

    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
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
        using handle_t = fmt::basic_format_arg<format_context>::handle;
        using custom_value_t = detail::custom_value<format_context>;
        static_assert_size_same(handle_t, custom_value_t);
        auto out = ctx.out();
        if (macho_arg_id >= 0) {
            auto macho_arg = ctx.arg(macho_arg_id);
            auto arg_handle = visit_format_arg(value_extractor<handle_t>(), macho_arg);
            auto custom_value = (custom_value_t *)&arg_handle;
            const auto &macho = *(const MachO *)custom_value->value;
            assert(macho.isMagicGood());
            return nlist.format_to(out, macho);
        }
        return nlist.format_to(out);
    }
};