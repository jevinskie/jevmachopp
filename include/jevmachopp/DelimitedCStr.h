#pragma once

#include "jevmachopp/Common.h"

#include <cstring>

template <char Delimiter> class DelimitedCStrIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char;
    using pointer = const char *;
    using reference = const char &;

    DelimitedCStrIterator() : m_ptr(nullptr) {}
    DelimitedCStrIterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    DelimitedCStrIterator &operator++() {
        const char *next_del = std::strchr(m_ptr, Delimiter);
        if (!next_del) {
            *this = std::move(DelimitedCStrIterator{});
        } else {
            m_ptr = next_del + 1;
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
    pointer m_ptr;
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
