#pragma once

#include "jevmachopp/Common.h"

#include <string_view>

class MachO;

namespace CallFinder {

bool findCallsTo(const MachO &macho, const std::string_view symbol_name);

} // namespace CallFinder
