#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Section.h"

#include <cstring>

#include "fmt/core.h"

std::string SegmentCommand::segName() const {
    return readMaybeNullTermCString<decltype(segname)>(segname);
}

const Section *SegmentCommand::sect_cbegin() const {
    return (const Section *)(this + 1);
}

const Section *SegmentCommand::sect_cend() const {
    return (const Section *)((uintptr_t)(this + 1) + sect_sizeof());
}

size_t SegmentCommand::sect_size() const {
    return nsects;
}

size_t SegmentCommand::sect_sizeof() const {
    return sect_size() * sizeof(Section);
}

SegmentCommand::sect_range SegmentCommand::sections() const {
    return {sect_cbegin(), sect_cend()};
}
