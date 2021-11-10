#pragma once

#include "jevmachopp/Packer.h"

class LoadSubCommand : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const = 0;
    uint64_t unpack(uint8_t *buf, uint8_t *base) = 0;
    uint64_t size() const = 0;
};
