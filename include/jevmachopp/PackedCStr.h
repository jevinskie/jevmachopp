#pragma once

#include "jevmachopp/Common.h"

#include <cstring>

template <bool EmptyStringTerminated = false> class PackedCStrIteratorBase {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const char;
    using pointer = const char *;
    using reference = const char &;

    PackedCStrIteratorBase() : m_ptr(nullptr) {}
    PackedCStrIteratorBase(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    PackedCStrIteratorBase &operator++() {
        m_ptr = m_ptr + std::strlen(m_ptr) + 1;
        if constexpr (EmptyStringTerminated) {
            // check if the string we're now pointing to is empty (the end)
            if (!std::strlen(m_ptr)) {
                *this = std::move(PackedCStrIteratorBase{});
            }
        }
        return *this;
    }
    PackedCStrIteratorBase operator++(int) {
        PackedCStrIteratorBase tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const PackedCStrIteratorBase &a, const PackedCStrIteratorBase &b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const PackedCStrIteratorBase &a, const PackedCStrIteratorBase &b) {
        return a.m_ptr != b.m_ptr;
    };

private:
    pointer m_ptr;
};

using PackedCStrIterator = PackedCStrIteratorBase<>;

using packed_cstr_range = subrange<PackedCStrIterator>;

using PackedCStrIteratorEmtpyTerm = PackedCStrIteratorBase<true>;

using packed_cstr_eterm_range = subrange<PackedCStrIteratorEmtpyTerm>;
