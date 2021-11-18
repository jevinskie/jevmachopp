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

// // Checks if an argument is a valid printf width specifier and sets
// // left alignment if it is negative.
template <typename Char> class my_handler {

 public:
  explicit my_handler() {}


  template <typename T, std::enable_if_t<std::is_integral<T>::value>>
  unsigned operator()(T value) {
    return static_cast<unsigned>(value);
  }

  template <typename T, std::enable_if_t<!std::is_integral<T>::value>>
  unsigned operator()(T) {
    return 0;
  }
};

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
                [=](auto val) {
                    using T = decltype(val);
                    if (std::is_same_v<T, int>) {
                        fmt::print("int\n");
                    }
                    if (std::is_same_v<T, const void *>) {
                        fmt::print("pointer\n");
                    }
                    if (std::is_same_v<T, fmt::basic_format_arg<format_context>::handle>) {
                        fmt::print("macho??\n");
                        // auto v = reinterpret_cast<const void*>(val);
                        // auto v = fmt::custom_value<format_context>(val);
                        // auto vz = detail::value<decltype(ctx)>{};
                        // auto v = detail::arg_mapper<decltype(ctx)>::map();
                        fmt::print("fark\n");
                        // auto v = static_cast<fmt::basic_format_arg<format_context>::handle>(val);
                    }
                    //                    auto v = custom.value;
                    fmt::print("fuck\n");
                },
                // my_handler(),
                macho_arg);

            auto bar = visit_format_arg(my_handler<char>(), macho_arg);
            fmt::print("");
        }
        return nlist.format_to(out);
    }
};
