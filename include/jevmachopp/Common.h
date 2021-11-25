#pragma once

#include <cstring>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <hedley.h>

#include <boost/callable_traits/function_type.hpp>
#include <boost/callable_traits/remove_member_cv.hpp>

#if !M1N1
#define BOOST_STATIC_STRING_THROW(ex)                                                              \
    do {                                                                                           \
        fprintf(stderr, "Exception: %s\n", ex);                                                    \
        abort();                                                                                   \
    } while (0)
#else
extern "C" __attribute__((noreturn)) void __assert_fail(const char *assertion, const char *file,
                                                        unsigned int line, const char *function);
#define BOOST_STATIC_STRING_THROW(ex) __assert_fail(ex, __FILE__, __LINE__, __func__)
#endif
#define BOOST_NORETURN [[noreturn]]

#include <boost/static_string/static_string.hpp>

#include <nanorange/algorithm/any_of.hpp>
#include <nanorange/algorithm/copy.hpp>
#include <nanorange/concepts.hpp>
#include <nanorange/views/empty.hpp>
#include <nanorange/views/filter.hpp>
#include <nanorange/views/subrange.hpp>
#include <nanorange/views/transform.hpp>
using namespace nano;

using namespace fmt::literals;
using namespace std::string_literals;
using namespace std::literals::string_view_literals;

#pragma mark Printing

#if defined(USE_FMT) && !USE_FMT
#define FMT_PRINT(...)
#else
#define FMT_PRINT(...) fmt::print(__VA_ARGS__)
#endif

#pragma mark Common Types

using dylib_names_map_t = std::array<const char *, 0xFF>;
using indirect_syms_idxes_t = std::span<const uint32_t>;

#pragma mark Type Utilities

template <typename T> constexpr auto type_name() {
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

#pragma mark Size Checking

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

#pragma mark static_assert helpers

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

#pragma mark Type Conversions

// Extracts the underlying type from an enum.
template <typename T, bool is_enum = std::is_enum<T>::value> struct ArithmeticOrUnderlyingEnum;

template <typename T> struct ArithmeticOrUnderlyingEnum<T, true> {
    using type = typename std::underlying_type<T>::type;
    static const bool value = std::is_arithmetic<type>::value;
};

template <typename T> struct ArithmeticOrUnderlyingEnum<T, false> {
    using type = T;
    static const bool value = std::is_arithmetic<type>::value;
};

template <typename T> struct UnderlyingType {
    using type = typename ArithmeticOrUnderlyingEnum<T>::type;
    static const bool is_numeric = std::is_arithmetic<type>::value;
    static const bool is_checked = false;
    static const bool is_clamped = false;
    static const bool is_strict = false;
};

// https://chromium.googlesource.com/chromium/src/+/refs/heads/main/base/numerics/safe_conversions_impl.h
// as_unsigned<> returns the supplied integral value (or integral castable
// Numeric template) cast as an unsigned integral of equivalent precision.
// I.e. it's mostly an alias for: static_cast<std::make_unsigned<T>::type>(t)
template <typename Src>
constexpr typename std::make_unsigned<typename UnderlyingType<Src>::type>::type
as_unsigned(const Src value) {
    static_assert(std::is_integral<decltype(as_unsigned(value))>::value,
                  "Argument must be a signed or unsigned integer type.");
    return static_cast<decltype(as_unsigned(value))>(value);
}

template <typename> struct remove_member_pointer;

template <typename U, typename F> struct remove_member_pointer<U F::*> {
    using member_type = U;
    using class_type = F;
};

#pragma mark Ranges

template <typename Rng, typename F>
const range_value_t<Rng> *find_if_or_nullptr(Rng &&rng, F pred) {
    auto res = ranges::find_if(rng, pred);
    if (res != std::end(rng)) {
        return &*res;
    }
    return nullptr;
}

template <typename Rng, typename Str> bool rangeContainsStr(Rng &&rng, Str &&strToFind) {
    const auto strToFindView = std::string_view{strToFind};
    return ranges::any_of(rng, [&strToFindView](const auto &str) {
        return str == strToFindView;
    });
}

#pragma mark fmt

template <std::size_t N>
struct fmt::formatter<boost::static_string<N>> : fmt::formatter<fmt::string_view> {
    auto format(const boost::static_string<N> &str, format_context &ctx) const
        -> decltype(ctx.out()) {
        return formatter<string_view>::format(str.data(), ctx);
    }
};

#pragma mark Utilities

template <typename Buf, std::size_t BufSz = sizeof(Buf)>
boost::static_string<BufSz> readMaybeNullTermCString(const char *cstr) {
    if (!cstr)
        return "";
    if (std::memchr(cstr, '\0', BufSz)) {
        return {cstr};
    } else {
        return {cstr, BufSz};
    }
}

template <typename T, typename G>
requires requires(T &ptr, G getter) {
    convertible_to<decltype(getter()), T>;
    std::is_pointer_v<T>;
}
T setIfNull(T &ptr, G getter) {
    if (ptr) {
        return ptr;
    }
    ptr = getter();
    return ptr;
}

#define setIfNullErroringRet(ptr, getter, errRes)                                                  \
    ({                                                                                             \
        if (!setIfNull((ptr), (getter))) {                                                         \
            return errRes;                                                                         \
        }                                                                                          \
        (ptr);                                                                                     \
    })
