#pragma once

#include <memory>

#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Packer.h"

class LoadCommand : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    LoadCommandType cmd;
    std::unique_ptr<LoadSubCommand> subcmd;
};
