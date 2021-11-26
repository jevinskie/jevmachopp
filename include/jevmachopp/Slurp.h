#pragma once

#include <iosfwd>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"

class Slurp {
public:
    static uint8_t *readfile(const char *filename, uint64_t *len, bool rw = true,
                             void *preferred_addr = (void *)0x400000000ull);
    static void writefile(const char *filename, uint8_t *buf, uint64_t len);
    static uint8_t *mapMemory(std::size_t size, void *preferred_addr = (void *)0x300000000ull);
};
