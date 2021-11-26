#pragma once

#include <iosfwd>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"

class Slurp {
public:
    static uint8_t *readfile(const char *filename, std::size_t *len = nullptr, bool rw = true,
                             const void *preferred_addr = (const void *)0x400000000ull);
    static void writefile(const char *filename, uint8_t *buf, std::size_t len);
    static uint8_t *mapMemory(std::size_t size,
                              const void *preferred_addr = (const void *)0x300000000ull);
};
