#pragma once

#include <memory>

#include "KCFS/Packer.h"
#include "KCFS/LoadCommandType.h"
#include "KCFS/LoadSubCommand.h"

class LoadCommand : public Packer {
public:
  uint64_t pack(uint8_t *buf, uint8_t *base) const override;
  uint64_t unpack(uint8_t *buf, uint8_t *base) override;
  uint64_t size() const override;

public:
  LoadCommandType cmd;
  std::unique_ptr<LoadSubCommand> subcmd;
};
