#pragma once

#include "jevmachopp/Common.h"

#include <uleb128/uleb128.h>

template <typename Int>
requires requires() {
    requires same_as<Int, uint64_t>;
}
class LEB128Iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Int;
    using pointer = Int *;
    using reference = Int &;

    LEB128Iterator() : m_buf({}), m_val(0), m_nbytes(0), m_idx(0) {}
    LEB128Iterator(std::span<const uint8_t> lebBuf)
        : m_buf(lebBuf), m_val(0), m_nbytes(0), m_idx(0) {}

    reference operator*() {
        updateVal();
        return m_val;
    }
    pointer operator->() {
        updateVal();
        return &m_val;
    }
    LEB128Iterator &operator++() {
        updateVal();
        m_idx += m_nbytes;
        assert(m_idx <= m_buf.size_bytes());
        if (m_idx == m_buf.size_bytes()) {
            *this = {};
        } else {
            clearVal();
        }
        return *this;
    }
    LEB128Iterator operator++(int) {
        LEB128Iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const LEB128Iterator &a, const LEB128Iterator &b) {
        return (a.m_buf.data() == b.m_buf.data() && a.m_buf.size() == b.m_buf.size()) &&
               a.m_idx == b.m_idx;
    };
    friend bool operator!=(const LEB128Iterator &a, const LEB128Iterator &b) {
        return (a.m_buf.data() != b.m_buf.data() || a.m_buf.size() != b.m_buf.size()) ||
               a.m_idx != b.m_idx;
    };

private:
    void updateVal() {
        if (!m_nbytes) {
            m_nbytes = bfs::DecodeUleb128(m_buf.subspan(m_idx), &m_val);
            assert(m_nbytes);
        }
    }

    void clearVal() {
        m_val = 0;
        m_nbytes = 0;
    }

private:
    std::span<const uint8_t> m_buf;
    Int m_val;
    std::size_t m_nbytes;
    std::size_t m_idx;
};
