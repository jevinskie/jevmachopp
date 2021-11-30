#include "jevmachopp/FunctionStartsCommand.h"

func_start_range FunctionStartsCommand::functionStarts() const {
    return {cbegin(), cend()};
}

FuncStartIterator FunctionStartsCommand::cbegin() const {
    return {data()};
}

FuncStartIterator FunctionStartsCommand::cend() const {
    return {};
}

#if USE_FMT
fmt::appender &FunctionStartsCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<FunctionStartsCommand>"_cf);
    return out;
}
#endif
