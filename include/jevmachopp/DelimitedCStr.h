#pragma once

#include "jevmachopp/Common.h"

#include <cstring>
#include <string_view>

// FIXME: use split_view https://en.cppreference.com/w/cpp/ranges/split_view

template <char Delimiter> class DelimitedCStrIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::string_view;
    using pointer           = std::string_view *;
    using reference         = std::string_view &;

    DelimitedCStrIterator() : m_ptr(nullptr) {}
    DelimitedCStrIterator(const char *delimitedCStr) : m_ptr(delimitedCStr) {
        set_view();
    }

    reference operator*() {
        return m_view;
    }
    pointer operator->() {
        return &m_view;
    }
    DelimitedCStrIterator &operator++() {
        const char *next_del = std::strchr(m_ptr, Delimiter);
        if (!next_del) {
            *this = std::move(DelimitedCStrIterator{});
        } else {
            m_ptr = next_del + 1;
            set_view();
        }
        return *this;
    }
    DelimitedCStrIterator operator++(int) {
        DelimitedCStrIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const DelimitedCStrIterator &a, const DelimitedCStrIterator &b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const DelimitedCStrIterator &a, const DelimitedCStrIterator &b) {
        return a.m_ptr != b.m_ptr;
    };

private:
    std::size_t size() const {
        return std::strlen(m_ptr);
    }

    void set_view(const char *next_del = nullptr) {
        if (!next_del) {
            next_del = std::strchr(m_ptr, Delimiter);
        }
        if (!next_del) {
            m_view = {m_ptr};
        } else {
            m_view = {m_ptr, (uintptr_t)next_del - (uintptr_t)m_ptr};
        }
    }

private:
    const char *m_ptr;
    std::string_view m_view;
};

template <char Delimiter>
subrange<DelimitedCStrIterator<Delimiter>> rangeForDelimitedCStr(const char *delimitedCStr) {
    return {DelimitedCStrIterator<Delimiter>{delimitedCStr}, DelimitedCStrIterator<Delimiter>{}};
}

using SpaceDelimitedCStrIterator = DelimitedCStrIterator<' '>;

using space_delimited_cstr_range = subrange<SpaceDelimitedCStrIterator>;

inline space_delimited_cstr_range rangeForSpaceDelimitedCStr(const char *delimitedCStr) {
    return rangeForDelimitedCStr<' '>(delimitedCStr);
}
