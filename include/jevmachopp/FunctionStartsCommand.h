#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

#include <nanorange/detail/views/range_adaptors.hpp>

// template <std::ranges::forward_range V>
// requires requires() {
//     requires std::ranges::view<V>;
// }
// class offset_partial_sum_view {
//     V base_ = V();
//     using B = decltype(base_.base());

// public:
//     offset_partial_sum_view() = default;
//     offset_partial_sum_view(V b) : base_(std::move(b)) { }

//     auto begin() -> std::ranges::iterator_t<V> { return ranges::begin(base_); }
//     auto end() -> std::ranges::iterator_t<V> { return ranges::end(&base_); }
// };

// template <typename t>
// using iterator_t = decltype(std::begin(std::declval<t &>()));

// template <typename t>
// using range_reference_t = decltype(*std::begin(std::declval<t &>()));

template <typename urng_t, typename C, typename Val = ranges::range_value_t<urng_t>>
requires requires(Val v, C off) {
    requires ranges::input_range<urng_t>;
    requires ranges::common_reference_with<range_reference_t<urng_t>, Val>;
    v += off;
}
class view_add_constant : public ranges::view_base {
private:
    /* data members == "the state" */
    struct data_members_t {
        urng_t urange;
    };
    data_members_t data_members;

    /* the iterator type */
    struct iterator_type : iterator_t<urng_t> {
        using base = iterator_t<urng_t>;
        using reference = Val;

        iterator_type() = default;
        iterator_type(base const &b) : base{b} {}

        iterator_type operator++(int) {
            return static_cast<base &>(*this)++;
        }

        iterator_type &operator++() {
            ++static_cast<base &>(*this);
            return (*this);
        }

        reference operator*() const {
            return *static_cast<base>(*this) + 42;
        }
    };

public:
    /* member type definitions */
    using reference = Val;
    using const_reference = Val;
    using value_type = Val;

    using iterator = iterator_type;
    using const_iterator = iterator_type;

    /* constructors and deconstructors */
    // TODO: why needed if defaulted? need2l2cpp
    view_add_constant() = default;
    constexpr view_add_constant(view_add_constant const &rhs) = default;
    constexpr view_add_constant(view_add_constant &&rhs) = default;
    constexpr view_add_constant &operator=(view_add_constant const &rhs) = default;
    constexpr view_add_constant &operator=(view_add_constant &&rhs) = default;
    ~view_add_constant() = default;

    view_add_constant(urng_t &&urange, C off = 0)
        : data_members{data_members_t{std::forward<urng_t>(urange)}} {}

    /* begin and end */
    iterator begin() const {
        return std::begin(data_members.urange);
    }
    iterator cbegin() const {
        return begin();
    }

    auto end() const {
        return std::end(data_members.urange);
    }

    auto cend() const {
        return end();
    }
};

template <typename urng_t, typename C, typename Val = ranges::range_value_t<urng_t>>
requires requires(Val v, C off) {
    requires ranges::input_range<urng_t>;
    requires ranges::common_reference_with<range_reference_t<urng_t>, Val>;
    v += off;
}
view_add_constant(urng_t &&, C)->view_add_constant<urng_t, C>;

namespace nano {
namespace detail {

struct add_constant_fn {
    template <typename C> constexpr auto operator()(C c) const {

        return detail::rao_proxy{
            [c = std::move(c)](auto &&r) mutable
            -> decltype(view_add_constant{std::forward<decltype(r)>(r), std::declval<C &&>()}) {
                return view_add_constant{std::forward<decltype(r)>(r), std::move(c)};
            }};
    }

    template <typename E, typename F>
    constexpr auto operator()(E &&e, F &&f) const
        -> decltype(view_add_constant{std::forward<E>(e), std::forward<F>(f)}) {
        return view_add_constant{std::forward<E>(e), std::forward<F>(f)};
    }
};

} // namespace detail
} // namespace nano

namespace view {

nano::detail::add_constant_fn constexpr add_constant;

}

class FunctionStartsCommand : public LinkeditDataCommand {
public:
    raw_func_start_range raw_offsets(const MachO &macho) const;
    FuncStartIterator raw_offsets_cbegin(const MachO &macho) const;
    FuncStartIterator raw_offsets_cend() const;

#if 0
    auto file_offsets(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        setIfNullAsserting(textSeg, [&]() {
            return macho.textSeg();
        });
        const uint64_t text_fileoff = textSeg->fileoff;
        uint64_t prev = text_fileoff;
        return raw_offsets(macho) |
               ranges::views::transform([prev, text_fileoff](const uint64_t raw_off) mutable {
                   prev += raw_off;
                   return prev;
               });
    }
#endif

    auto file_offsets(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        (void)textSeg;
        return raw_offsets(macho) | view::add_constant(5);
    }

    // auto vm_addrs(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
    //     setIfNullAsserting(textSeg, [&]() {
    //         return macho.textSeg();
    //     });
    //     const auto vmaddr_fileoff_delta = textSeg->vmaddr_fileoff_delta();
    //     return file_offsets(macho, textSeg) | ranges::views::transform([&](const auto &file_off)
    //     {
    //         return file_off + vmaddr_fileoff_delta;
    //     });
    // }

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    FunctionStartsCommand(const FunctionStartsCommand &) = delete;
    void operator=(const FunctionStartsCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(FunctionStartsCommand, struct linkedit_data_command,
                                     struct load_command);
#endif
static_assert_size_is(FunctionStartsCommand, 8);

#if USE_FMT
template <> struct fmt::formatter<FunctionStartsCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(FunctionStartsCommand const &funcStartsCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return funcStartsCmd.format_to(out);
    }
};
#endif
