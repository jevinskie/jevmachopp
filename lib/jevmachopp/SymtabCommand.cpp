#include "jevmachopp/SymtabCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

fmt::appender &SymtabCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<SymtabCommand @ {:p}>"_cf, (void *)loadCommand());
    return out;
}
