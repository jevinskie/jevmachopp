#pragma once

#include "jevmachopp/Common.h"

#include <boost/static_string/static_string.hpp>
#include <string_view>

uint8_t ascii_hex_to_nibble(const uint8_t chr);
uint8_t nibble_to_ascii_hex(const uint8_t chr);
uint8_t ascii_hex_to_byte(const char *hex);
void byte_to_ascii_hex(uint8_t n, char *hex);

template <std::size_t BufSz>
boost::static_string<BufSz * 2> buf2hexstr(const void *buf, const std::size_t buf_sz) {
    boost::static_string<BufSz * 2> hexstr{};
    std::span<const std::uint8_t> binbuf{(const uint8_t *)buf, std::min(buf_sz, BufSz)};
    for (const std::uint8_t n : binbuf) {
        char hexbytebuf[2];
        byte_to_ascii_hex(n, hexbytebuf);
        hexstr += std::string_view{hexbytebuf, sizeof(hexbytebuf)};
    }
    return hexstr;
}

template <typename HexStrBuf, std::size_t BufSz = HexStrBuf::static_capacity>
void buf2hexstr(const void *buf, const std::size_t buf_sz, HexStrBuf &hexstr) {
    std::span<const std::uint8_t> binbuf{(const uint8_t *)buf, std::min(buf_sz, BufSz / 2)};
    for (const std::uint8_t n : binbuf) {
        char hexbytebuf[2];
        byte_to_ascii_hex(n, hexbytebuf);
        hexstr += std::string_view{hexbytebuf, sizeof(hexbytebuf)};
    }
    return hexstr;
}
