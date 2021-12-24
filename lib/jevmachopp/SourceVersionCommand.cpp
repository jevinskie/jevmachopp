#include "jevmachopp/SourceVersionCommand.h"

#if USE_FMT
fmt::appender &SourceVersionCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<SourceVersionCommand version: {}>"_cf, version);
    return out;
}
#endif
