#pragma once

#include "jevmachopp/Common.h"

#include <nanorange/detail/views/range_adaptors.hpp>

template <typename Rng, typename Val = ranges::range_value_t<Rng>>
requires requires() {
    requires ranges::input_range<Rng>;
    requires ranges::common_reference_with<range_reference_t<Rng>, Val>;
}
class psum_view : public ranges::view_base {
private:
    struct data_members_t {
        Rng urange;
        Val init;
    };
    data_members_t data_members;

    /* the iterator type */
    struct iterator_type : iterator_t<Rng> {
        using base = iterator_t<Rng>;
        using reference = Val;
        Val psum;

        iterator_type() = default;
        iterator_type(base const &b, Val i) : base{b}, psum{i} {}

        iterator_type operator++(int) {
            psum += *static_cast<base>(*this);
            return static_cast<base &>(*this)++;
        }

        iterator_type &operator++() {
            psum += *static_cast<base>(*this);
            ++static_cast<base &>(*this);
            return (*this);
        }

        reference operator*() const {
            return *static_cast<base>(*this) + psum;
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
    psum_view() = default;
    constexpr psum_view(psum_view const &rhs) = default;
    constexpr psum_view(psum_view &&rhs) = default;
    constexpr psum_view &operator=(psum_view const &rhs) = default;
    constexpr psum_view &operator=(psum_view &&rhs) = default;
    ~psum_view() = default;

    psum_view(Rng &&urange, Val init = 0)
        : data_members{data_members_t{std::forward<Rng>(urange), std::forward<Val>(init)}} {}

    /* begin and end */
    iterator begin() const {
        return iterator(std::begin(data_members.urange), data_members.init);
    }
    iterator cbegin() const {
        return begin();
    }

    auto end() const {
        return iterator(std::end(data_members.urange), data_members.init);
    }

    auto cend() const {
        return end();
    }
};

template <typename Rng, typename Val = ranges::range_value_t<Rng>>
requires requires() {
    requires ranges::input_range<Rng>;
    requires ranges::common_reference_with<range_reference_t<Rng>, Val>;
}
psum_view(Rng &&, Val)->psum_view<Rng>;

namespace nano {
namespace detail {

struct psum_fn {
    template <typename Val> constexpr auto operator()(Val init) const {
        return detail::rao_proxy{
            [init = std::move(init)](auto &&r) mutable
            -> decltype(psum_view{std::forward<decltype(r)>(r), std::declval<Val &&>()}) {
                return psum_view{std::forward<decltype(r)>(r), std::move(init)};
            }};
    }

    template <typename E, typename F>
    constexpr auto operator()(E &&e, F &&f) const
        -> decltype(psum_view{std::forward<E>(e), std::forward<F>(f)}) {
        return psum_view{std::forward<E>(e), std::forward<F>(f)};
    }
};

} // namespace detail
} // namespace nano

namespace view {

nano::detail::psum_fn constexpr psum;

}
