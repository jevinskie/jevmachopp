#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/DelimitedCStr.h"
#include "jevmachopp/PackedCStr.h"

#include <span>
#include <string_view>

#pragma mark CHRPNVRAMHeader

class CHRPNVRAMHeader {

public:
    uint16_t size_blocks() const;
    uint32_t size_bytes() const;
    const char *name() const;
    const uint8_t *data() const;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    uint8_t sig;
    uint8_t chksum;
    uint16_t len;
    char name_buf[12]; // assume NUL termination because lazy

public:
    CHRPNVRAMHeader(const CHRPNVRAMHeader &) = delete;
    void operator=(const CHRPNVRAMHeader &) = delete;
};

static_assert_size_is(CHRPNVRAMHeader, 16);

#if USE_FMT
template <> struct fmt::formatter<CHRPNVRAMHeader> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(CHRPNVRAMHeader const &chrpHdr, FormatContext &ctx) {
        auto out = ctx.out();
        return chrpHdr.format_to(out);
    }
};
#endif

#pragma mark AppleNVRAMHeader

class AppleNVRAMHeader {
public:
    CHRPNVRAMHeader chrp_hdr;
    uint32_t adler;
    uint32_t generation;
    uint64_t padding;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    AppleNVRAMHeader(const AppleNVRAMHeader &) = delete;
    void operator=(const AppleNVRAMHeader &) = delete;
};

static_assert_size_is(AppleNVRAMHeader, 32);

#if USE_FMT
template <> struct fmt::formatter<AppleNVRAMHeader> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(AppleNVRAMHeader const &appleHdr, FormatContext &ctx) {
        auto out = ctx.out();
        return appleHdr.format_to(out);
    }
};
#endif

class NVRAMPartition {
public:
    const char *name() const;
    uint32_t size_bytes() const;

public:
    packed_cstr_eterm_range vars() const;
    PackedCStrIteratorEmtpyTerm vars_cbegin() const;
    PackedCStrIteratorEmtpyTerm vars_cend() const;
    const char *varNamed(const char *name) const;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    const CHRPNVRAMHeader &hdr;

public:
    NVRAMPartition(const CHRPNVRAMHeader &hdr) : hdr(hdr) {}
};

// static_assert_size_same(NVRAMPartition, CHRPNVRAMHeader);

#if USE_FMT
template <> struct fmt::formatter<NVRAMPartition> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(NVRAMPartition const &part, FormatContext &ctx) {
        auto out = ctx.out();
        return part.format_to(out);
    }
};
#endif

class NVRAMProxyData {
public:
    space_delimited_cstr_range bootArgs() const;
    bool hasBootArg(const char *argName) const;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    const AppleNVRAMHeader &nvram_hdr;
    const NVRAMPartition common_part;
    const NVRAMPartition system_part;

public:
    NVRAMProxyData(const AppleNVRAMHeader &nvram_hdr, const NVRAMPartition &common_part,
                   const NVRAMPartition &system_part)
        : nvram_hdr(nvram_hdr), common_part(common_part), system_part(system_part) {}
};

#if USE_FMT
template <> struct fmt::formatter<NVRAMProxyData> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(NVRAMProxyData const &nvramProxyData, FormatContext &ctx) {
        auto out = ctx.out();
        return nvramProxyData.format_to(out);
    }
};
#endif

namespace NVRAM {

const std::string_view varName(const char *varEqValStr);
const char *varValue(const char *varEqValStr);
NVRAMProxyData extractProxyData(const void *nvram_proxy_data_buf);
uint32_t decodedDataLen(const std::span<const uint8_t> &escaped);
uint32_t unescapeBytesToData(const std::span<const uint8_t> &escaped, std::span<uint8_t> &&decoded);
uint32_t escapeDataToData(const std::span<const uint8_t> &decoded, std::span<uint8_t> &encoded);

} // namespace NVRAM
