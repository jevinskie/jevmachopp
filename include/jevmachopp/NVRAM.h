#pragma once

#include "jevmachopp/Common.h"

#include <span>

class CHRPNVRAMHeader {

public:
    uint8_t sig;
    uint8_t chksum;
    uint16_t len;
    char name[12]; // assume NUL termination because lazy

public:
    CHRPNVRAMHeader(const CHRPNVRAMHeader &) = delete;
    void operator=(const CHRPNVRAMHeader &) = delete;
};

static_assert_size_is(CHRPNVRAMHeader, 16);

class AppleNVRAMHeader {
public:
    CHRPNVRAMHeader chrp_hdr;
    uint32_t adler;
    uint32_t generation;
    uint64_t padding;

public:
    AppleNVRAMHeader(const AppleNVRAMHeader &) = delete;
    void operator=(const AppleNVRAMHeader &) = delete;
};

static_assert_size_is(AppleNVRAMHeader, 32);

namespace NVRAM {

uint32_t decodedDataLen(const std::span<const uint8_t> &escaped);
uint32_t unescapeBytesToData(const std::span<const uint8_t> &escaped, std::span<uint8_t> &&decoded);
uint32_t escapeDataToData(const std::span<const uint8_t> &decoded, std::span<uint8_t> &encoded);

} // namespace NVRAM
