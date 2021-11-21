#pragma once

#include <iosfwd>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"

class Slurp {
public:
    static uint8_t *readfile(const char *filename, uint64_t *len,
                             void *preferred_addr = (void *)0x400000000ull);
    static void writefile(const char *filename, uint8_t *buf, uint64_t len);
};
