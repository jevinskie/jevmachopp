#pragma once

#include "jevmachopp/Common.h"

#pragma mark DTNode

class DTNode {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint32_t nprops;
    uint32_t nchildren;

public:
    DTNode(const DTNode &) = delete;
    void operator=(const DTNode &) = delete;
};

static_assert_size_is(DTNode, 8);

#if USE_FMT
template <> struct fmt::formatter<DTNode> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(DTNode const &node, FormatContext &ctx) {
        auto out = ctx.out();
        return node.format_to(out);
    }
};
#endif

#pragma mark DTProp

class DTProp {
public:
    auto name() const {
        return readMaybeNullTermCString<decltype(name_buf)>(name_buf);
    }
    const uint8_t *data() const;
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    char name_buf[32];
    uint32_t sz;

public:
    DTProp(const DTProp &) = delete;
    void operator=(const DTProp &) = delete;
};

static_assert_size_is(DTProp, 36);

#if USE_FMT
template <> struct fmt::formatter<DTProp> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(DTProp const &prop, FormatContext &ctx) {
        auto out = ctx.out();
        return prop.format_to(out);
    }
};
#endif
