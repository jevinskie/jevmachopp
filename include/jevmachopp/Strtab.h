#pragma once

#include "jevmachopp/Common.h"

#include <cstring>

class StrtabIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char;
    using pointer = const char *;
    using reference = const char &;

    StrtabIterator() : m_ptr(nullptr) {}
    StrtabIterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    StrtabIterator &operator++() {
        m_ptr = m_ptr + std::strlen(m_ptr) + 1;
        return *this;
    }
    StrtabIterator operator++(int) {
        StrtabIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const StrtabIterator &a, const StrtabIterator &b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const StrtabIterator &a, const StrtabIterator &b) {
        return a.m_ptr != b.m_ptr;
    };

private:
    pointer m_ptr;
};

using strtab_entry_range = subrange<StrtabIterator>;
