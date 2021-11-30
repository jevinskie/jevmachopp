#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/LoadSubCommand.h"

using FuncStartIterator = LEB128Iterator<uint64_t>;
using func_start_range = subrange<FuncStartIterator>;

class FunctionStartsCommand : public LinkeditDataCommand {
public:
    func_start_range functionStarts() const;
    FuncStartIterator cbegin() const;
    FuncStartIterator cend() const;

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    FunctionStartsCommand(const FunctionStartsCommand &) = delete;
    void operator=(const FunctionStartsCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(FunctionStartsCommand, struct linkedit_data_command,
                                     struct load_command);
#endif
static_assert_size_is(FunctionStartsCommand, 8);

#if USE_FMT
template <> struct fmt::formatter<FunctionStartsCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(FunctionStartsCommand const &funcStartsCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return funcStartsCmd.format_to(out);
    }
};
#endif
