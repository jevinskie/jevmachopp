#include "jevmachopp/LinkeditDataCommand.h"

fmt::appender &LinkeditDataCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<LinkeditDataCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}