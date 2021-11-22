#include "jevmachopp/LinkeditDataCommand.h"

#if USE_FMT
fmt::appender &LinkeditDataCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<LinkeditDataCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
#endif
