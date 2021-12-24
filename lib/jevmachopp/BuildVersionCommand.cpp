#include "jevmachopp/BuildVersionCommand.h"

#if USE_FMT
fmt::appender &BuildVersionCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out,
                   "<BuildVersionCommand platform: {:#010x} minos: {} sdk: {} ntools: {:d}>"_cf,
                   platform, minos, sdk, ntools);
    return out;
}
#endif
