#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

std::span<const uint8_t> LinkeditDataCommand::data(const MachO *macho,
                                                   const SegmentCommand *linkeditSeg) const {
    assert(macho);
    setIfNullAsserting(linkeditSeg, [=]() {
        return macho->linkeditSeg();
    });
    const auto ledata = linkeditSeg->data(*macho);
    return {&ledata[dataoff], datasize};
}

#if USE_FMT
fmt::appender &LinkeditDataCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<LinkeditDataCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
#endif
