#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

#include <nanorange/views/empty.hpp>

using FuncStartIterator = LEB128Iterator<uint64_t>;
using raw_func_start_range = subrange<FuncStartIterator>;
using func_start_range = subrange<uint64_t>;

class FunctionStartsCommand : public LinkeditDataCommand {
public:
    raw_func_start_range raw_offsets(const MachO &macho) const;
    FuncStartIterator raw_offsets_cbegin(const MachO &macho) const;
    FuncStartIterator raw_offsets_cend() const;

    auto file_offsets(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        setIfNullAsserting(textSeg, [&]() {
            return macho.textSeg();
        });
        bool first = true;
        uint64_t prev = 0;
        return raw_offsets(macho) | ranges::views::transform([&](const auto &raw_off) {
                   if (first) {
                       first = false;
                       prev = raw_off + textSeg->fileoff;
                   } else {
                       prev += raw_off;
                   }
                   return prev;
               });
    }

    // auto vm_addrs(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
    //     setIfNullAsserting(textSeg, [&]() {
    //         return macho.textSeg();
    //     });
    //     const auto vmaddr_fileoff_delta = textSeg->vmaddr_fileoff_delta();
    //     return file_offsets(macho, textSeg) | ranges::views::transform([&](const auto &file_off)
    //     {
    //         return file_off + vmaddr_fileoff_delta;
    //     });
    // }

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
