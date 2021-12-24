#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

std::span<const uint8_t> LinkeditDataCommand::data(const MachO &macho) const {
    return {(const uint8_t *)&macho + dataoff, datasize};
}

#if USE_FMT
fmt::appender &LinkeditDataCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<LinkeditDataCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
#endif
