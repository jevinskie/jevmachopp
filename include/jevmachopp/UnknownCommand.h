#pragma once

#include <vector>
#include <cstdint>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UnknownCommand : public LoadSubCommand {
  public:
    int dummy;
};

