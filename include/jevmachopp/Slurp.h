#pragma once

#include <iosfwd>
#include <stdint.h>
#include <string>

#include "jevmachopp/Common.h"

class Slurp {
  public:
    static uint8_t *readfile(const std::string &filename, uint64_t *len);
    static void writefile(const std::string &filename, uint8_t *buf, uint64_t len);
};
