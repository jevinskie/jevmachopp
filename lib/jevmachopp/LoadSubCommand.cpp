#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

#include "jevmachopp/EncryptionInfoCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/UUIDCommand.h"
#include "jevmachopp/UnknownCommand.h"

#include <visit.hpp>

const LoadCommand *LoadSubCommand::loadCommand() const {
    return (const LoadCommand *)((uintptr_t)this - sizeof(LoadCommand));
}

const SubCommandVariant LoadSubCommand::get() const {
    switch (loadCommand()->cmd) {
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
    rollbear::visit(
        [=](auto &&o) {
            fmt::format_to(out, "{}"_cf, *o);
        },
        get());
    return out;
}
