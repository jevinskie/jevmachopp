#include "jevmachopp/UnknownCommand.h"
#include "jevmachopp/LoadCommand.h"

fmt::appender &UnknownCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<UnknownCommand @ {:p} type: {:#010x}>", (void *)loadCommand(),
                   as_unsigned(loadCommand()->cmd));
    return out;
}