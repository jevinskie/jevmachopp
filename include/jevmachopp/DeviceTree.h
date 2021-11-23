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
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const DTProp;
        using pointer = const DTProp *;
        using reference = const DTProp &;

        Iterator() : m_ptr(nullptr) {}
        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const {
            return *m_ptr;
        }
        pointer operator->() {
            return m_ptr;
        }
        Iterator &operator++() {
            m_ptr = (pointer)((uintptr_t)m_ptr + m_ptr->sz);
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator &a, const Iterator &b) {
            return a.m_ptr == b.m_ptr;
        };
        friend bool operator!=(const Iterator &a, const Iterator &b) {
            return a.m_ptr != b.m_ptr;
        };

    private:
        pointer m_ptr;
    };

public:
    const char *name() const;
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
