#pragma once

#include "jevmachopp/Packer.h"

class LoadSubCommand : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;
};
