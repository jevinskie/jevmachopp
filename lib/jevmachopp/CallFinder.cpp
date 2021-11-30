#include "jevmachopp/CallFinder.h"
#include "jevmachopp/ARM64Disasm.h"
#include "jevmachopp/CallStubs.h"
#include "jevmachopp/FunctionStartsCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/Section.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/LEB128.h"

#include <concepts>

namespace CallFinder {

static constexpr std::uint32_t BYTES_PER_STUB = 16;


} // namespace CallFinder
