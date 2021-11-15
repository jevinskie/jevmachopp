#pragma once

#include <mach-o/loader.h>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UUIDCommand : public LoadSubCommand {
  public:
    uint8_t uuid[16];
};

static_assert_size_same_minus_header(UUIDCommand, struct uuid_command, struct load_command);
