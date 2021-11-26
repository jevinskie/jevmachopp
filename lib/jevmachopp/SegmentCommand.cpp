#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Section.h"

#include <cstring>

const Section *SegmentCommand::sect_cbegin() const {
    return (const Section *)(this + 1);
}

const Section *SegmentCommand::sect_cend() const {
    return (const Section *)((uintptr_t)(this + 1) + sect_sizeof());
}

std::size_t SegmentCommand::sect_size() const {
    return nsects;
}

std::size_t SegmentCommand::sect_sizeof() const {
    return sect_size() * sizeof(Section);
}

std::span<const Section> SegmentCommand::sections() const {
    return {sect_cbegin(), sect_cend()};
}

AddrRange SegmentCommand::vmaddr_range() const {
    return {vmaddr, vmaddr + vmsize};
}

AddrRange SegmentCommand::file_range() const {
    return {fileoff, fileoff + filesize};
}
