#pragma once

#include "jevmachopp/Common.h"

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