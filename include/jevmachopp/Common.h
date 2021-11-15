#pragma once

#include <hedley.h>
#include <utility>
#include <type_traits>

template <typename T> constexpr T abs_diff(T a, T b) {
    return a > b ? a - b : b - a;
}

template <typename TA, typename TB, std::size_t SizeA = sizeof(TA), std::size_t SizeB = sizeof(TB)>
constexpr std::size_t get_size_diff() {
    return abs_diff(SizeA, SizeB);
}

template <typename T, std::size_t SizeExpected, std::size_t SizeActual = sizeof(T)>
constexpr bool check_size_is() {
    static_assert(SizeActual == SizeExpected, "Size is off!");
    return false;
}

template <typename TA, typename TB, std::size_t SizeA = sizeof(TA), std::size_t SizeB = sizeof(TB)>
constexpr bool check_size_equal() {
    static_assert(SizeA == SizeB, "Size is off!");
    return false;
}

template <typename TA, typename TB, typename TBH, std::size_t SizeA = sizeof(TA),
          std::size_t SizeB = sizeof(TB), std::size_t SizeBH = sizeof(TBH)>
constexpr bool check_size_minus_header_equal() {
    static_assert(SizeA == SizeB - SizeBH, "Size is off!");
    return false;
}

#define static_assert_cond(cond) static_assert((cond), #cond)

#define static_assert_size_is(obj, sz)                                                             \
    __attribute__((unused)) static constexpr bool HEDLEY_CONCAT(                                   \
        static_assert_size_is_, __COUNTER__) = check_size_is<obj, (sz)>()

#define static_assert_size_same(obj_a, obj_b)                                                      \
    __attribute__((unused)) static constexpr bool HEDLEY_CONCAT(                                   \
        static_assert_size_same_, __COUNTER__) = check_size_equal<obj_a, obj_b>()

#define static_assert_size_same_minus_header(obj_a, obj_b, obj_b_hdr)                              \
    __attribute__((unused)) static constexpr bool HEDLEY_CONCAT(                                   \
        static_assert_size_same_minus_header_, __COUNTER__) =                                      \
        check_size_minus_header_equal<obj_a, obj_b, obj_b_hdr>()

template <typename E> constexpr auto to_underlying_int(E e) noexcept {
    return +(static_cast<std::underlying_type_t<E>>(e));
}

// template <typename Src>
// constexpr typename std::make_unsigned<
//     typename base::internal::UnderlyingType<Src>::type>::type
// as_unsigned(const Src value) {
//   static_assert(std::is_integral<decltype(as_unsigned(value))>::value,
//                 "Argument must be a signed or unsigned integer type.");
//   return static_cast<decltype(as_unsigned(value))>(value);
// }

// https://chromium.googlesource.com/chromium/src/+/refs/heads/main/base/numerics/safe_conversions_impl.h
// as_unsigned<> returns the supplied integral value (or integral castable
// Numeric template) cast as an unsigned integral of equivalent precision.
// I.e. it's mostly an alias for: static_cast<std::make_unsigned<T>::type>(t)
template <typename Src>
constexpr std::make_unsigned_t<std::underlying_type_t<Src>>
as_unsigned(const Src value) {
    static_assert(std::is_integral<decltype(as_unsigned(value))>::value,
                  "Argument must be a signed or unsigned integer type.");
    return static_cast<decltype(as_unsigned(value))>(value);
}
