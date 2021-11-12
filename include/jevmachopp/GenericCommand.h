#pragma once

#include "jevmachopp/LoadSubCommand.h"
#include <vector>

class GenericCommand : public LoadSubCommand {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    std::vector<uint8_t> data;
};