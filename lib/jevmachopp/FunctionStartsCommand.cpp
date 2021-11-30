#include "jevmachopp/FunctionStartsCommand.h"

func_start_range FunctionStartsCommand::offsets_raw(const MachO &macho) const {
    return {offsets_raw_cbegin(macho), offsets_raw_cend()};
}

FuncStartIterator FunctionStartsCommand::offsets_raw_cbegin(const MachO &macho) const {
    return {data(macho)};
}

FuncStartIterator FunctionStartsCommand::offsets_raw_cend() const {
    return {};
}

#if USE_FMT
fmt::appender &FunctionStartsCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<FunctionStartsCommand>"_cf);
    return out;
}
#endif
