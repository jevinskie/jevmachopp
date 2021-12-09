#pragma once

#include <atomic>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

#if USE_FMT
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#endif
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

#if USE_FMT
// for custom fmt printer
#include <boost/static_string/static_string.hpp>
#endif

#include <nanorange/algorithm/any_of.hpp>
#include <nanorange/views/filter.hpp>
#include <nanorange/views/split.hpp>
#include <nanorange/views/subrange.hpp>
#include <nanorange/views/transform.hpp>
using namespace nano;

#if USE_FMT
using namespace fmt::literals;
#endif
using namespace std::literals::string_view_literals;

#pragma mark Printing

#if defined(USE_FMT) && !USE_FMT
#define FMT_PRINT(...)
#else
#define FMT_PRINT(...) fmt::print(__VA_ARGS__)
#endif

struct sv2pf {
    const char *str;
    int sz;
    sv2pf(const std::string_view &sv) : str(sv.data()), sz((int)sv.size()) {}
};

// FIXME: convert rest of project from sv2pf -> SV2PF
// #define SV2PF(sv) sv2pf((sv)).sz, sv2pf((sv)).str
#define SV2PF(sv) (int)((sv).size()), (sv).data()

#pragma mark Simple Types

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
using int128_t = __int128;
using uint128_t = unsigned __int128;
#pragma GCC diagnostic pop

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

consteval std::size_t sizeofbits(const auto &o) {
    return sizeof(o) * 8;
}

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

#pragma mark Spans

//
// make_span() - Utility functions for creating spans
//
// https://github.com/microsoft/GSL/blob/main/include/gsl/span_ext
template <class ElementType>
constexpr std::span<ElementType> make_span(ElementType *ptr,
                                           typename std::span<ElementType>::size_type count) {
    return std::span<ElementType>(ptr, count);
}

template <class ElementType>
constexpr std::span<ElementType> make_span(ElementType *firstElem, ElementType *lastElem) {
    return std::span<ElementType>(firstElem, lastElem);
}

template <class ElementType, std::size_t N>
constexpr std::span<ElementType, N> make_span(ElementType (&arr)[N]) noexcept {
    return std::span<ElementType, N>(arr);
}

template <class Container>
constexpr std::span<typename Container::value_type> make_span(Container &cont) {
    return std::span<typename Container::value_type>(cont);
}

template <class Container>
constexpr std::span<const typename Container::value_type> make_span(const Container &cont) {
    return std::span<const typename Container::value_type>(cont);
}

template <class Ptr>
constexpr std::span<typename Ptr::element_type> make_span(Ptr &cont, std::size_t count) {
    return std::span<typename Ptr::element_type>(cont, count);
}

template <class Ptr> constexpr std::span<typename Ptr::element_type> make_span(Ptr &cont) {
    return std::span<typename Ptr::element_type>(cont);
}

template <class ElementType, std::size_t N>
constexpr auto make_span_from_back(ElementType (&arr)[N], std::size_t off) noexcept {
    const auto span_ = std::span<ElementType, N>(arr);
    return span_.subspan(0, span_.size() - off);
}

#pragma mark Ranges

template <typename Rng, typename F>
const range_value_t<Rng> *find_if_or_nullptr(Rng &&rng, F pred) {
    auto res = ranges::find_if(rng, pred);
    if (res != std::end(rng)) {
        return &*res;
    }
    return nullptr;
}

template <typename Rng, typename F> std::optional<ssize_t> get_index_in(Rng &&rng, F pred) {
    auto res = ranges::find_if(rng, pred);
    if (res != std::end(rng)) {
        return ranges::distance(rng.begin(), res);
    }
    return {};
}

template <typename Rng> bool rangeContainsStr(Rng &&rng, std::string_view strToFind) {
    return ranges::any_of(rng, [strToFind](const auto &str) {
        return str == strToFind;
    });
}

inline auto stringSplitViewDelimitedBy(std::string_view to_split, char delim) {
    return to_split | views::split(delim) | views::transform([](const auto &nsv) {
               return std::string_view{&*nsv.begin(), (size_t)ranges::distance(nsv)};
           });
}

template <typename NewT, typename T> std::span<NewT> span_cast(std::span<T> orig) {
    assert(orig.size_bytes() % sizeof(NewT) == 0);
    return {(NewT *)orig.data(), orig.size_bytes() / sizeof(NewT)};
}

template <typename NewT, typename T> std::span<const NewT> span_cast(std::span<const T> orig) {
    assert(orig.size_bytes() % sizeof(NewT) == 0);
    return {(const NewT *)orig.data(), orig.size_bytes() / sizeof(NewT)};
}

#pragma mark fmt

#if USE_FMT
template <std::size_t N>
struct fmt::formatter<boost::static_string<N>> : fmt::formatter<fmt::string_view> {
    auto format(const boost::static_string<N> &str, format_context &ctx) const
        -> decltype(ctx.out()) {
        return formatter<string_view>::format(str.data(), ctx);
    }
};
#endif

#pragma mark printf

#define PI64 "%" PRId64
#define PU64 "%" PRIu64
#define PX64 "0x%" PRIx64
#define PX64F "0x%016" PRIx64

#pragma mark Utilities

std::string_view readMaybeNullTermCString(const char *cstr, std::size_t buf_sz);

template <typename T, typename G>
requires requires(T &ptr, G getter) {
    requires convertible_to<decltype(getter()), T>;
    requires std::is_pointer_v<T>;
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

#define setIfNullAsserting(ptr, getter) assert(setIfNull((ptr), (getter)))

template <typename T>
requires requires(T v) {
    (bool)v;
}
constexpr const char *YESNO(T v) {
    if (v) {
        return "YES";
    } else {
        return "NO";
    }
}

template <typename T>
requires requires(T v) {
    (bool)v;
}
constexpr const char *YESNOCStr(T v) {
    return YESNO(v);
}

template <typename T>
requires requires(T v) {
    (bool)v;
}
constexpr const std::string_view YESNO(T v) {
    if (v) {
        return "YES"sv;
    } else {
        return "NO"sv;
    }
}

template <typename I>
requires requires() {
    requires integral<I>;
}
constexpr I gcd(I a, I b) {
    I tmpb;
    if constexpr (std::is_signed_v<I>) {
        a = std::abs(a);
        b = std::abs(b);
    }
    while (b != 0) {
        tmpb = b;
        b = a % b;
        a = tmpb;
    }
    return a;
}

template <typename I>
requires requires() {
    requires integral<I>;
}
constexpr I lcm(I a, I b) {
    I c = gcd(a, b);
    return c == 0 ? 0 : a / c * b;
}

template <typename I>
requires requires() {
    requires integral<I>;
}
constexpr bool is_pow2(I num) {
    auto nbits_set = 0;
    for (int i = 0; i < sizeof(I) * 8; ++i) {
        if (((decltype(as_unsigned(num)))1 << i) & as_unsigned(num)) {
            ++nbits_set;
        }
    }
    return nbits_set == 1;
}

template <typename U>
requires requires() {
    requires unsigned_integral<U>;
}
constexpr U roundup_pow2(U num, unsigned int nth_pow2) {
    U mask = (static_cast<U>(1) << nth_pow2) - 1;
    return (num + mask) & ~mask;
}

template <typename U>
requires requires() {
    requires unsigned_integral<U>;
}
constexpr U roundup_pow2_mul(U num, std::size_t pow2_mul) {
    U mask = static_cast<U>(pow2_mul) - 1;
    return (num + mask) & ~mask;
}

template <typename T> constexpr std::add_volatile_t<T> &make_volatile(T &o) {
    return (decltype(make_volatile(o)))(o);
}
