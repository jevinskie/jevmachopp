#include "jevmachopp/DylibCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

const char *DylibCommand::name() const {
    return (const char *)((uintptr_t)loadCommand() + name_offset);
}

bool DylibCommand::isID() const {
    return loadCommand()->cmd == LoadCommandType::ID_DYLIB;
}

bool DylibCommand::isLoad() const {
    return loadCommand()->cmd == LoadCommandType::LOAD_DYLIB;
}

bool DylibCommand::isLoadWeak() const {
    return loadCommand()->cmd == LoadCommandType::LOAD_WEAK_DYLIB;
}

bool DylibCommand::isRexport() const {
    return loadCommand()->cmd == LoadCommandType::REEXPORT_DYLIB;
}

DylibCommand::Version::Version(uint32_t ver)
    : major(ver >> 16), minor((ver >> 8) & 0xff), patch(ver & 0xff) {}

DylibCommand::Version DylibCommand::currentVersion() const {
    return current_version;
}

DylibCommand::Version DylibCommand::compatibilityVersion() const {
    return compatibility_version;
}

fmt::appender &DylibCommand::format_to(fmt::appender &out) const {
    fmt::format_to(
        out,
        "<DylibCommand @ {:p} \"{:s}\" timestamp: {:#010x} current_version: {} compatibility_version: {}>",
        (void *)loadCommand(), name(), timestamp, currentVersion(), compatibilityVersion());
    return out;
}
