#include "jevmachopp/FunctionStartsCommand.h"

#include <uleb128/uleb128.h>

#if USE_FMT
fmt::appender &FunctionStartsCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<FunctionStartsCommand>"_cf);
    return out;
}
#endif
