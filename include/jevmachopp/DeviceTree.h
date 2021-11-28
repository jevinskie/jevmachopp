#pragma once

#include "jevmachopp/Common.h"

#include <experimental/fixed_capacity_vector>
#include <string_view>

#pragma mark DTRegister

class DTRegister {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    const void *base;
    std::size_t size;

public:
    DTRegister() : base(nullptr), size(0) {}
    DTRegister(const void *base, std::size_t size) : base(base), size(size) {}
    DTRegister(const DTRegister &) = delete;
};

static_assert_size_is(DTRegister, 16);

#if USE_FMT
template <> struct fmt::formatter<DTRegister> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(DTRegister const &reg, FormatContext &ctx) {
        auto out = ctx.out();
        return reg.format_to(out);
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

        Iterator() : m_ptr(nullptr), m_sz(0) {}
        Iterator(pointer ptr, std::uint32_t sz) : m_ptr(ptr), m_sz(sz) {
            if (!sz) {
                *this = Iterator{};
            }
        }

        reference operator*() const {
            return *m_ptr;
        }
        pointer operator->() {
            return m_ptr;
        }
        Iterator &operator++() {
            m_ptr = (pointer)((uintptr_t)(m_ptr + 1) + m_ptr->size_padded());
            --m_sz;
            if (!m_sz) {
                *this = Iterator{};
            }
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator &a, const Iterator &b) {
            bool same_ptr = a.m_ptr == b.m_ptr;
            assert(!same_ptr || a.m_sz == b.m_sz);
            return same_ptr;
        };
        friend bool operator!=(const Iterator &a, const Iterator &b) {
            bool diff_ptr = a.m_ptr != b.m_ptr;
            assert(diff_ptr || a.m_sz == b.m_sz);
            return diff_ptr;
        };

    private:
        pointer m_ptr;
        std::uint32_t m_sz;
    };

public:
    using prop_range = subrange<DTProp::Iterator>;
    static constexpr std::uint32_t PROP_SIZE_REPLACEMENT_TAG = 0x80000000u;

public:
    const char *name() const;
    void setName(const char *newName);
    std::uint32_t size_raw() const;
    std::uint32_t size_padded() const;
    const uint8_t *data() const;
    bool isReplacement() const;

    const char *as_cstr() const;
    const uint32_t &as_u32() const;
    const uint64_t &as_u64() const;
    const DTRegister &as_reg() const;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    char name_buf[32];
    std::uint32_t sz;

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

#pragma mark DTNode

class DTNode {
    // TODO: templatize and remove all these copies of Iterator
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const DTNode;
        using pointer = const DTNode *;
        using reference = const DTNode &;

        Iterator() : m_ptr(nullptr), m_sz(0) {}
        Iterator(pointer ptr, std::uint32_t sz) : m_ptr(ptr), m_sz(sz) {
            if (!sz) {
                *this = Iterator{};
            }
        }

        reference operator*() const {
            return *m_ptr;
        }
        pointer operator->() {
            return m_ptr;
        }
        Iterator &operator++() {
            m_ptr = (pointer)((uintptr_t)(m_ptr + 1) + m_ptr->properties_sizeof());
            --m_sz;
            if (!m_sz) {
                *this = Iterator{};
            }
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator &a, const Iterator &b) {
            bool same_ptr = a.m_ptr == b.m_ptr;
            assert(same_ptr && a.m_sz == b.m_sz);
            return same_ptr;
        };
        friend bool operator!=(const Iterator &a, const Iterator &b) {
            bool diff_ptr = a.m_ptr != b.m_ptr;
            assert(diff_ptr || a.m_sz == b.m_sz);
            return diff_ptr;
        };

    private:
        pointer m_ptr;
        std::uint32_t m_sz;
    };

public:
    using child_range = subrange<DTNode::Iterator>;

public:
    DTProp::prop_range properties() const;
    DTProp::Iterator properties_cbegin() const;
    DTProp::Iterator properties_cend() const;
    std::uint32_t properties_size() const;
    std::uint32_t properties_sizeof() const;
    const DTProp *propertyNamed(const std::string_view &name) const;
    auto properties_sized(std::uint32_t size) const {
        return ranges::views::filter(properties(), [=](const DTProp &prop) {
            return prop.size_raw() == size;
        });
    }
    auto propertiesNamedWithPrefix(const char *prefix) const {
        return ranges::views::filter(properties(), [=](const DTProp &prop) {
            return std::string_view{prop.name()}.starts_with(prefix);
        });
    }

    DTNode::child_range children() const;
    DTNode::Iterator children_cbegin() const;
    DTNode::Iterator children_cend() const;
    std::uint32_t children_size() const;
    std::uint32_t children_sizeof() const;
    const DTNode *childNamed(const std::string_view &name) const;

    void lookup(const char *path) const;

    const char *name() const;
    const char *name_or_nullptr() const;
    std::uint32_t node_sizeof() const;

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

namespace DT {

constexpr std::size_t MAX_CPUS = 16;
std::experimental::fixed_capacity_vector<const uint64_t *, MAX_CPUS>
getCPUImplRegAddrs(const DTNode &rootNode);

} // namespace DT
