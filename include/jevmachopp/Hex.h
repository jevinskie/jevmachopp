#pragma once

#include "jevmachopp/Common.h"

#include <boost/static_string/static_string.hpp>
#include <nanorange/algorithm/all_of.hpp>
#include <nanorange/views/reverse.hpp>
#include <string_view>

uint8_t ascii_hex_to_nibble(const uint8_t chr);
uint8_t nibble_to_ascii_hex(const uint8_t chr);
uint8_t ascii_hex_to_byte(const char *hex);
void byte_to_ascii_hex(uint8_t n, char *hex);
bool is_hex_digit(const uint8_t chr);
uint8_t ascii_digit_to_int(const uint8_t chr);
bool is_decimal_digit(const uint8_t chr);

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

template <typename Int>
requires requires(Int res) {
    !std::is_same_v<Int, bool>;
    std::is_integral_v<Int>;
}
std::optional<Int> sv2int(std::string_view sv) {
    Int res = 0;
    if (sv.size() > 2 && sv.starts_with("0x"sv)) {
        const auto hexdigits = sv.substr(2);
        if (hexdigits.size() > sizeof(res) * 2) {
            // (potential) overflow
            return {};
        }
        if (!ranges::all_of(hexdigits, is_hex_digit)) {
            return {};
        }
        for (const char hd : hexdigits) {
            res <<= 4;
            res |= ascii_hex_to_nibble(hd);
        }
        return res;
    } else {
        // efficient way to calculate overflow for decimal goes here
        if (!ranges::all_of(sv, is_decimal_digit)) {
            return {};
        }
        for (const char d : sv) {
            res *= 10;
            res += ascii_digit_to_int(d);
        }
        return res;
    }
    return {};
};