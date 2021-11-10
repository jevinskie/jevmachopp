#pragma once

#include "jevmachopp/LoadSubCommand.h"
#include <vector>

class EncryptionInfoCommand : public LoadSubCommand {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const override;
    uint64_t unpack(uint8_t *buf, uint8_t *base) override;
    uint64_t size() const override;

  public:
    uint32_t cryptid;
    std::vector<uint8_t> data;
};
