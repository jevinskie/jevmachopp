#pragma once

#include "jevmachopp/Common.h"

#pragma mark DTreeNode

class DTreeNode {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint32_t nprops;
    uint32_t nchildren;

public:
    DTreeNode(const DTreeNode &) = delete;
    void operator=(const DTreeNode &) = delete;
};

static_assert_size_is(DTreeNode, 8);

#if USE_FMT
template <> struct fmt::formatter<DTreeNode> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(DTreeNode const &node, FormatContext &ctx) {
        auto out = ctx.out();
        return node.format_to(out);
    }
};
#endif

#pragma mark DTreeProp

class DTreeProp {
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
    DTreeProp(const DTreeProp &) = delete;
    void operator=(const DTreeProp &) = delete;
};

static_assert_size_is(DTreeProp, 36);

#if USE_FMT
template <> struct fmt::formatter<DTreeProp> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(DTreeProp const &prop, FormatContext &ctx) {
        auto out = ctx.out();
        return prop.format_to(out);
    }
};
#endif
