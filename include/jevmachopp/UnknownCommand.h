#pragma once

#include <cstdint>
#include <vector>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class UnknownCommand : public LoadSubCommand {
public:
    int dummy;
};
