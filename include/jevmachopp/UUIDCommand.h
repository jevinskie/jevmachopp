#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UUIDCommand : public LoadSubCommand {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    uint8_t uuid[16];
};
