#pragma once

#include "jevmachopp/Common.h"

#include <cstring>

#if 0
class StrtabIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char *;
    using pointer = const char **;
    using reference = const char *&;

    Iterator() : m_ptr(nullptr) {}
    Iterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    Iterator &operator++() {
        m_ptr = m_ptr + std::strlen(m_ptr) + 1;
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
#endif
