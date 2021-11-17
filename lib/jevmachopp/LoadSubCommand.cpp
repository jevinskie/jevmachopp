#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/DylibCommand.h"
#include "jevmachopp/EncryptionInfoCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/SymtabCommand.h"
#include "jevmachopp/UUIDCommand.h"
#include "jevmachopp/UnknownCommand.h"

const LoadCommand *LoadSubCommand::loadCommand() const {
    return (const LoadCommand *)((uintptr_t)this - sizeof(LoadCommand));
}

const SubCommandVariant LoadSubCommand::get() const {
    switch (loadCommand()->cmd) {
    case LoadCommandType::SYMTAB:
        return SubCommandVariant{(const SymtabCommand *)this};
    case LoadCommandType::DYSYMTAB:
        return SubCommandVariant{(const DySymtabCommand *)this};
    case LoadCommandType::ID_DYLIB:
    case LoadCommandType::LOAD_DYLIB:
    case LoadCommandType::LOAD_WEAK_DYLIB:
    case LoadCommandType::REEXPORT_DYLIB:
        return SubCommandVariant{(const DylibCommand *)this};
    case LoadCommandType::ENCRYPTION_INFO:
        return SubCommandVariant{(const EncryptionInfoCommand *)this};
    case LoadCommandType::SEGMENT_64:
        return SubCommandVariant{(const SegmentCommand *)this};
    case LoadCommandType::UUID:
        return SubCommandVariant{(const UUIDCommand *)this};
    default:
        return SubCommandVariant{(const UnknownCommand *)this};
    }
}

fmt::appender &LoadSubCommand::format_to(fmt::appender &out) const {
    std::visit(
        [=](auto &&o) {
            fmt::format_to(out, "{}"_cf, *o);
        },
        get());
    return out;
}
