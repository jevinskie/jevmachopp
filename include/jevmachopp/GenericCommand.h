#pragma once

#include <vector>
#include <cstdint>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class GenericCommand : public LoadSubCommand {
  public:

  public:
    std::vector<uint8_t> data;
};
