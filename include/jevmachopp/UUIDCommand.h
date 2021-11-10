#pragma once

#include "jevmachopp/LoadSubCommand.h"

class UUIDCommand : public LoadSubCommand {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const override;
    uint64_t unpack(uint8_t *buf, uint8_t *base) override;
    uint64_t size() const override;

  public:
    uint8_t uuid[16];
};
