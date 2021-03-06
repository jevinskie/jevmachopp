#include "jevmachopp/UnknownCommand.h"
#include "jevmachopp/LoadCommand.h"

#if USE_FMT
fmt::appender &UnknownCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<UnknownCommand @ {:p} type: {:#010x}>"_cf, (void *)loadCommand(),
                   as_unsigned(loadCommand()->cmd));
    return out;
}
#endif
