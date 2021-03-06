#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Section.h"

#include <cstring>

std::string_view SegmentCommand::name() const {
    return readMaybeNullTermCString(segname, sizeof(segname));
}

std::span<const uint8_t> SegmentCommand::data(const MachO &macho) const {
    return {(const uint8_t *)&macho + fileoff, filesize};
}

const Section *SegmentCommand::sect_cbegin() const {
    return (const Section *)(this + 1);
}

const Section *SegmentCommand::sect_cend() const {
    return (const Section *)((uintptr_t)(this + 1) + sect_sizeof());
}

uint32_t SegmentCommand::sect_size() const {
    return nsects;
}

std::size_t SegmentCommand::sect_sizeof() const {
    return sect_size() * sizeof(Section);
}

std::span<const Section> SegmentCommand::sections() const {
    return {sect_cbegin(), sect_cend()};
}

const Section *SegmentCommand::sectionNamed(const std::string_view sect_name) const {
    return find_if_or_nullptr(sections(), [=](const auto &sect) {
        return sect.sectName() == sect_name;
    });
}

AddrRange SegmentCommand::vmaddr_range() const {
    return {vmaddr, vmaddr + vmsize};
}

AddrRange SegmentCommand::file_range() const {
    return {fileoff, fileoff + filesize};
}

uint64_t SegmentCommand::vmaddr_fileoff_delta() const {
    return vmaddr - fileoff;
}
