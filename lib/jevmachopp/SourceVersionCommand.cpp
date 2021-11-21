#include "jevmachopp/SourceVersionCommand.h"

fmt::appender &SourceVersionCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<SourceVersionCommand version: {}>"_cf, version);
    return out;
}
