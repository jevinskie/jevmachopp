#pragma once

#include "jevmachopp/Common.h"

#include <cstdint>
#include <experimental/fixed_capacity_vector>
#include <string_view>

#include <enum.hpp/enum.hpp>

using jev_csr_config_t = std::uint32_t;

// clang-format off
ENUM_HPP_CLASS_DECL(CSRConfig, jev_csr_config_t,
    (ALLOW_UNTRUSTED_KEXTS            = (1 <<  0))
    (ALLOW_UNRESTRICTED_FS            = (1 <<  1))
    (ALLOW_TASK_FOR_PID               = (1 <<  2))
    (ALLOW_KERNEL_DEBUGGER            = (1 <<  3))
    (ALLOW_APPLE_INTERNAL             = (1 <<  4))
    (ALLOW_UNRESTRICTED_DTRAC         = (1 <<  5))
    (ALLOW_UNRESTRICTED_NVRAM         = (1 <<  6))
    (ALLOW_DEVICE_CONFIGURATION       = (1 <<  7))
    (ALLOW_ANY_RECOVERY_OS            = (1 <<  8))
    (ALLOW_UNAPPROVED_KEXTS           = (1 <<  9))
    (ALLOW_EXECUTABLE_POLICY_OVERRIDE = (1 << 10))
    (ALLOW_UNAUTHENTICATED_ROOT       = (1 << 11))
)
// clang-format on

inline std::experimental::fixed_capacity_vector<std::string_view, sizeofbits(CSRConfig{})>
to_strings(CSRConfig const &val) {
    decltype(to_strings(val)) res;
    constexpr auto dummy = decltype(val){};
    constexpr auto sz = CSRConfig_traits::size;
    constexpr auto bitsz = sizeofbits(dummy);
    static_assert(sz <= bitsz);

    for (size_t i = 0, e = sz; i < e; ++i) {
        auto i_enum = CSRConfig_traits::from_index_or_default(i, dummy);
        if (as_unsigned(i_enum) & as_unsigned(val)) {
            res.emplace_back(CSRConfig_traits::to_string_or_empty(i_enum));
        }
    }

    return res;
}

#if USE_FMT
template <> struct fmt::formatter<CSRConfig> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CSRConfig const &conf, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "<CSRConfig {:s} ({:#010x})>"_cf,
                              CSRConfig_traits::to_string_or_empty(conf), as_unsigned(conf));
    }
};
#endif
