#pragma once

#include <mach-o/loader.h>
#include <stdint.h>
#include <vector>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class EncryptionInfoCommand : public LoadSubCommand {
  public:
    uint32_t cryptoff;
    uint32_t cryptsize;
    uint32_t cryptid;
    uint32_t pad;
};

static_assert_size_same_minus_header(EncryptionInfoCommand, struct encryption_info_command_64, struct load_command);
