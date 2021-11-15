#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include <vector>

class GenericCommand : public LoadSubCommand {
  public:

  public:
    std::vector<uint8_t> data;
};
