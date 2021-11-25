#pragma once

#include "jevmachopp/Common.h"

#include <cstring>

class PackedCStrIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char;
    using pointer = const char *;
    using reference = const char &;

    PackedCStrIterator() : m_ptr(nullptr) {}
    PackedCStrIterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    PackedCStrIterator &operator++() {
        m_ptr = m_ptr + std::strlen(m_ptr) + 1;
        return *this;
    }
    PackedCStrIterator operator++(int) {
        PackedCStrIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const PackedCStrIterator &a, const PackedCStrIterator &b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const PackedCStrIterator &a, const PackedCStrIterator &b) {
        return a.m_ptr != b.m_ptr;
    };

private:
    pointer m_ptr;
};

using packed_cstr_range = subrange<PackedCStrIterator>;
