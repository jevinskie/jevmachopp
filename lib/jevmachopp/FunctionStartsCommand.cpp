#include "jevmachopp/FunctionStartsCommand.h"

#pragma mark raw offsets

raw_func_start_range FunctionStartsCommand::raw_offsets(const MachO &macho) const {
    return {raw_offsets_cbegin(macho), raw_offsets_cend()};
}

FuncStartIterator FunctionStartsCommand::raw_offsets_cbegin(const MachO &macho) const {
    return {data(macho)};
}

FuncStartIterator FunctionStartsCommand::raw_offsets_cend() const {
    return {};
}

#if USE_FMT
fmt::appender &FunctionStartsCommand::format_to(fmt::appender &out) const {
    fmt::format_to(out, "<FunctionStartsCommand>"_cf);
    return out;
}
#endif
