#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/DylibCommand.h"
#include "jevmachopp/EncryptionInfoCommand.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/SymtabCommand.h"
#include "jevmachopp/UUIDCommand.h"
#include "jevmachopp/UnknownCommand.h"

#include "rollbear/visit.hpp"

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
    case LoadCommandType::CODE_SIGNATURE:
    case LoadCommandType::SEGMENT_SPLIT_INFO:
    case LoadCommandType::FUNCTION_STARTS:
    case LoadCommandType::DATA_IN_CODE:
    case LoadCommandType::DYLIB_CODE_SIGN_DRS:
    case LoadCommandType::LINKER_OPTIMIZATION_HINT:
    case LoadCommandType::DYLD_EXPORTS_TRIE:
    case LoadCommandType::DYLD_CHAINED_FIXUPS:
        return SubCommandVariant{(const LinkeditDataCommand *)this};
    case LoadCommandType::SEGMENT_64:
        return SubCommandVariant{(const SegmentCommand *)this};
    case LoadCommandType::UUID:
        return SubCommandVariant{(const UUIDCommand *)this};
    default:
        return SubCommandVariant{(const UnknownCommand *)this};
    }
}

fmt::appender &LoadSubCommand::format_to(fmt::appender &out) const {
    rollbear::visit(
        [=](auto &&o) {
            fmt::format_to(out, "{}"_cf, *o);
        },
        get());
    return out;
}
