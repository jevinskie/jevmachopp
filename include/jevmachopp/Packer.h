#pragma once

#include <stdint.h>

class Packer {
  public:
    virtual uint64_t pack(uint8_t *buf, uint8_t *base) const = 0;
    virtual uint64_t unpack(uint8_t *buf, uint8_t *base) = 0;
    virtual uint64_t size() const = 0;
};
