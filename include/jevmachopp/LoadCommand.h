#pragma once

#include <cstddef>
#include <iterator>
#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"

class LoadCommand {
public:
    LoadCommand(const LoadCommand &) = delete;
    void operator=(const LoadCommand &) = delete;

public:
    const LoadSubCommand *subcmd() const;

public:
    LoadCommandType cmd;
    uint32_t cmdsize;

public:
    // https://internalpointers.com/post/writing-custom-iterators-modern-cpp
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = const LoadCommand;
        using pointer           = const LoadCommand *;
        using reference         = const LoadCommand &;

        Iterator() : m_ptr(nullptr) {}
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

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same(LoadCommand, struct load_command);
#endif
static_assert_size_is(LoadCommand, 8);

#if USE_FMT
template <> struct fmt::formatter<LoadCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadCommand const &lc, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<LoadCommand @ {:p} {} {}>"_cf, (void *)&lc, lc.cmd,
                              *lc.subcmd());
    }
};
#endif
