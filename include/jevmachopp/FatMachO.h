#pragma once

#include "jevmachopp/Common.h"

class FatMachO {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    FatMachO(const FatMachO &) = delete;
    void operator=(const FatMachO &) = delete;
};

#if USE_FMT
template <> struct fmt::formatter<FatMachO> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(FatMachO const &fatMachO, FormatContext &ctx) {
        auto out = ctx.out();
        return fatMachO.format_to(out);
    }
};
#endif
