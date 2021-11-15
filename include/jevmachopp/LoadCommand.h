#pragma once

#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Packer.h"

class LoadCommand : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;
    LoadSubCommand *subcmd() const;

  public:
    LoadCommandType cmd;
    uint32_t cmdsize;
};

static_assert_size_same(LoadCommand, struct load_command);
