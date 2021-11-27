#pragma once

#include "jevmachopp/Common.h"

using dylib_names_map_t = std::array<const char *, 0xFF>;
using indirect_syms_idxes_t = std::span<const uint32_t>;

class AddrRange {
public:
    AddrRange()
        : min(std::numeric_limits<decltype(min)>::max()),
          max(std::numeric_limits<decltype(max)>::min()){};
    AddrRange(uint64_t min, uint64_t max) : min(min), max(max) {
        assert(min <= max);
    }

    bool isNull() const {
        return min == std::numeric_limits<decltype(min)>::max() &&
               max == std::numeric_limits<decltype(max)>::min();
    }

    constexpr std::size_t size() const {
        return (std::size_t)(max - min);
    }

    AddrRange &operator|=(const AddrRange &rhs) {
        (void)rhs;
        return *this;
    }
    friend AddrRange operator|(AddrRange lhs, const AddrRange &rhs) {
        if (rhs.isNull()) {
            return lhs;
        }
        if (lhs.isNull()) {
            return rhs;
        }
        lhs.min = std::min(lhs.min, rhs.min);
        lhs.max = std::max(lhs.max, rhs.max);
        return lhs;
    };

    AddrRange &operator-=(const uint64_t rhs) {
        (void)rhs;
        return *this;
    }
    friend AddrRange operator-(AddrRange lhs, const uint64_t rhs) {
        if (lhs.isNull()) {
            return lhs;
        }
        lhs.min -= rhs;
        lhs.max -= rhs;
        return lhs;
    };

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint64_t min;
    uint64_t max;
};

#if USE_FMT
template <> struct fmt::formatter<AddrRange> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(AddrRange const &addrRange, FormatContext &ctx) {
        auto out = ctx.out();
        return addrRange.format_to(out);
    }
};
#endif
