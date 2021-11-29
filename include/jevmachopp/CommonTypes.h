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

    friend bool operator==(const AddrRange &lhs, const AddrRange &rhs) {
        return lhs.min == rhs.min && lhs.max == rhs.max;
    }
    friend bool operator!=(const AddrRange &lhs, const AddrRange &rhs) {
        return !(lhs == rhs);
    }

    AddrRange &operator|=(const AddrRange &rhs) {
        if (rhs.isNull()) {
            return *this;
        }
        if (isNull()) {
            *this = rhs;
        } else {
            min = std::min(min, rhs.min);
            max = std::max(max, rhs.max);
        }
        return *this;
    }
    friend AddrRange operator|(AddrRange lhs, const AddrRange &rhs) {
        return lhs |= rhs;
    }

    AddrRange &operator-=(const uint64_t rhs) {
        if (isNull()) {
            return *this;
        }
        min -= rhs;
        max -= rhs;
        return *this;
    }
    friend AddrRange operator-(AddrRange lhs, const uint64_t rhs) {
        return lhs -= rhs;
    }

    // template <typename Rng, std::enable_if_t<std::is_same_v<range_value_t<Rng>, AddrRange>>>
    template <typename Rng> static AddrRange collapseRange(Rng &&rng) {
        AddrRange res{};
        for (const auto &rhs_rng : rng) {
            res |= rhs_rng;
        }
        return res;
    }

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
