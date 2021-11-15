#pragma once

#include <utility>
#include <hedley.h>



template<typename T>
constexpr T abs_diff(T a, T b) {
  return a > b ? a - b : b - a;
}

template <typename TA, typename TB, std::size_t SizeA = sizeof(TA), std::size_t SizeB = sizeof(TB)>
constexpr std::size_t get_size_diff() {
  return abs_diff(SizeA, SizeB);
}

template <typename TA, typename TB, std::size_t SizeA = sizeof(TA), std::size_t SizeB = sizeof(TB)>
constexpr bool check_size() {
  static_assert(SizeA == SizeB, "Size is off!");
  return false;
}

template <typename TA, typename TB, typename TBH, std::size_t SizeA = sizeof(TA), std::size_t SizeB = sizeof(TB), std::size_t SizeBH = sizeof(TBH)>
constexpr bool check_size_minus_header() {
  static_assert(SizeA == SizeB - SizeBH, "Size is off!");
  return false;
}

#define static_assert_cond(cond) static_assert((cond), #cond)

#define static_assert_size_same(obj_a, obj_b) __attribute__((unused)) static constexpr bool HEDLEY_CONCAT(static_assert_size_same_,__COUNTER__) = check_size<obj_a, obj_b>()

#define static_assert_size_same_minus_header(obj_a, obj_b, obj_b_hdr) __attribute__((unused)) static constexpr bool HEDLEY_CONCAT(static_assert_size_same_minus_header_,__COUNTER__) = check_size_minus_header<obj_a, obj_b, obj_b_hdr>()




template <typename E> constexpr auto to_underlying_int(E e) noexcept {
    return +(static_cast<std::underlying_type_t<E>>(e));
}

