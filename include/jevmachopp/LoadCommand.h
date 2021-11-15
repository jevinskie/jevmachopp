#pragma once

#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"

class LoadCommand {
  public:
    LoadSubCommand *subcmd() const;

  public:
    LoadCommandType cmd;
    uint32_t cmdsize;
};

static_assert_size_same(LoadCommand, struct load_command);
