#include "jevmachopp/DylibCommand.h"

const char *DylibCommand::name() const {
    return (const char *)((uintptr_t)loadCommand() + name_offset);
}

fmt::appender &DylibCommand::format_to(fmt::appender &out) const {
    fmt::format_to(
        out,
        "<DylibCommand @ {:p} \"{:s}\" timestamp: {:#010x} current_version: {:#010x} compatibility_version: {:#010x}>"_cf,
        (void *)loadCommand(), name(), timestamp, current_version, compatibility_version);
    return out;
}
