#pragma once

#include <cstddef>
#include <iterator>
#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"

class LoadCommand {
public:
    const LoadSubCommand *subcmd() const;

public:
    LoadCommandType cmd;
    uint32_t cmdsize;

public:
    // https://internalpointers.com/post/writing-custom-iterators-modern-cpp
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const LoadCommand;
        using pointer = const LoadCommand *;
        using reference = const LoadCommand &;

        Iterator(pointer ptr) : m_ptr(ptr) {}

        reference operator*() const {
            return *m_ptr;
        }
        pointer operator->() {
            return m_ptr;
        }
        Iterator &operator++() {
            m_ptr = (pointer)((uintptr_t)m_ptr + m_ptr->cmdsize);
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
};

static_assert_size_same(LoadCommand, struct load_command);

template <> struct fmt::formatter<LoadCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadCommand const &lc, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<LoadCommand @ {:p} {}>", (void *)this, lc.cmd);
    }
};