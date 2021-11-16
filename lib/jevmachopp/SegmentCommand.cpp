#include "jevmachopp/SegmentCommand.h"

#include <cstring>

#include "fmt/core.h"

std::string SegmentCommand::segName() const {
    if (std::memchr(segname, '\0', sizeof(segname))) {
        return std::string{segname};
    } else {
        return std::string{segname, sizeof(segname)};
    }
}
