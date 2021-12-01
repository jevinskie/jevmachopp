#pragma once

#include "jevmachopp/Common.h"

#include "jevmachopp/ARM64Disasm.h"
#include "jevmachopp/CallStubs.h"
#include "jevmachopp/CommonTypes.h"
#include "jevmachopp/FunctionStartsCommand.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/Section.h"
#include "jevmachopp/SegmentCommand.h"

#include <string_view>

class MachO;

namespace CallFinder {

bool findCallsTo(const MachO &macho, const std::string_view symbol_name);

} // namespace CallFinder
