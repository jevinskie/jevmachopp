#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

#if USE_FMT
fmt::appender &DySymtabCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<DySymtabCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
#endif
