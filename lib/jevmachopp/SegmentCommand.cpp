#include "jevmachopp/SegmentCommand.h"

#include <cstring>

#include "fmt/core.h"

std::string SegmentCommand::segName() const {
    return readMaybeNullTermCString<decltype(segname)>(segname);
}
