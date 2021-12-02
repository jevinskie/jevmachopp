#pragma once

#include "jevmachopp/Common.h"

#include <nanorange/detail/views/range_adaptors.hpp>

template <typename V, typename Val = ranges::range_value_t<V>>
requires requires() {
    requires ranges::input_range<V>;
    requires ranges::common_reference_with<range_reference_t<V>, Val>;
}
class psum_view : public view_interface<psum_view<V>> {
private:
    V uview;
    Val init;

    /* the iterator type */
    struct iterator_type : iterator_t<V> {
        using base = iterator_t<V>;
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

        iterator_type operator--(int) {
            psum -= *static_cast<base>(*this);
            return static_cast<base &>(*this)--;
        }

        iterator_type &operator--() {
            psum -= *static_cast<base>(*this);
            --static_cast<base &>(*this);
            return (*this);
        }

        reference operator*() const {
            return psum;
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

    psum_view(V &&uview, Val init = 0)
        : uview{std::forward<V>(uview)}, init{std::forward<Val>(init)} {}

    /* begin and end */
    iterator begin() const {
        return iterator{std::begin(uview), init};
    }
    iterator cbegin() const {
        return begin();
    }

    auto end() const {
        return iterator{std::end(uview), init};
    }

    auto cend() const {
        return end();
    }
};

template <typename V, typename Val = ranges::range_value_t<V>>
requires requires() {
    requires ranges::input_range<V>;
    requires ranges::common_reference_with<range_reference_t<V>, Val>;
}
psum_view(V &&, Val)->psum_view<V>;

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
