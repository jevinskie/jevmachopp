#pragma once

#include "jevmachopp/Common.h"

#include <experimental/fixed_capacity_vector>

typedef struct chrp_nvram_header { // 16 bytes
    uint8_t sig;
    uint8_t cksum; // checksum on sig, len, and name
    uint16_t len;  // total length of the partition in 16 byte blocks starting with the signature
    // and ending with the last byte of data area, ie len includes its own header size
    char name[12];
    uint8_t data[0];
} chrp_nvram_header_t;

typedef struct apple_nvram_header { // 16 + 16 bytes
    struct chrp_nvram_header chrp;
    uint32_t adler;
    uint32_t generation;
    uint8_t padding[8];
} apple_nvram_header_t;

uint32_t unescapeBytesToData(const std::span<const uint8_t> &escaped, std::span<uint8_t> &&decoded);
uint32_t escapeDataToData(const std::span<const uint8_t> &decoded, std::span<uint8_t> &encoded);
