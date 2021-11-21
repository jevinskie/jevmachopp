#include "jevmachopp/FilesetEntryCommand.h"

const char *FilesetEntryCommand::name() const {
    return (const char *)((uintptr_t)loadCommand() + name_offset);
}

fmt::appender &FilesetEntryCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out,
                   "<FilesetEntryCommand @ {:p} vmaddr: {:#018x} fileoff: {:#010x} \"{:s}\" >"_cf,
                   (void *)loadCommand(), vmaddr, fileoff, name());
    return out;
}