#include "jevmachopp/Hex.h"

uint8_t ascii_hex_to_nibble(const uint8_t chr) {
    if (chr >= '0' && chr <= '9') {
        return chr - '0';
    } else if (chr >= 'a' && chr <= 'f') {
        return chr - 'a' + 0xA;
    } else {
        __builtin_unreachable();
    }
}

uint8_t nibble_to_ascii_hex(const uint8_t chr) {
    if (chr <= 9) {
        return chr + '0';
    } else if (chr >= 0xA && chr <= 0xF) {
        return chr - 0xA + 'a';
    } else {
        __builtin_unreachable();
    }
}

uint8_t ascii_hex_to_byte(const char *hex) {
    return (ascii_hex_to_nibble(hex[0]) << 4) | ascii_hex_to_nibble(hex[1]);
}

void byte_to_ascii_hex(uint8_t n, char *hex) {
    hex[0] = nibble_to_ascii_hex(n >> 4);
    hex[1] = nibble_to_ascii_hex(n & 0xF);
}

uint8_t ascii_digit_to_int(const uint8_t chr) {
    return chr - '0';
}

bool is_decimal_digit(const uint8_t chr) {
    return (chr >= '0' && chr <= '9');
}

bool is_hex_digit(const uint8_t chr) {
    return is_decimal_digit(chr) || (chr >= 'a' && chr <= 'f');
}
