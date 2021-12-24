#pragma once

#include "jevmachopp/Common.h"

class Version {
public:
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint8_t patch;
    uint8_t minor;
    uint16_t major;

public:
    Version(uint32_t ver);
    Version(uint16_t major, uint8_t minor, uint8_t patch);
};

#if USE_FMT
template <> struct fmt::formatter<Version> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    auto format(Version const &ver, format_context &ctx) -> decltype(ctx.out()) {
        auto out = ctx.out();
        return ver.format_to(out);
    }
};
#endif

class SourceVersion {
public:
    uint32_t a() const;
    uint16_t b() const;
    uint16_t c() const;
    uint16_t d() const;
    uint16_t e() const;
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint64_t version; // A.B.C.D.E packed as a24.b10.c10.d10.e10

public:
    SourceVersion(uint64_t version);
};

#if USE_FMT
template <> struct fmt::formatter<SourceVersion> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    auto format(SourceVersion const &srcVer, format_context &ctx) -> decltype(ctx.out()) {
        auto out = ctx.out();
        return srcVer.format_to(out);
    }
};
#endif
