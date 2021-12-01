#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/SegmentCommand.h"

class FunctionStartsCommand : public LinkeditDataCommand {
public:
    raw_func_start_range raw_offsets(const MachO &macho) const;
    FuncStartIterator raw_offsets_cbegin(const MachO &macho) const;
    FuncStartIterator raw_offsets_cend() const;

#if 1
    auto file_offsets(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        setIfNullAsserting(textSeg, [&]() {
            return macho.textSeg();
        });
        bool first = true;
        const uint64_t text_fileoff = textSeg->fileoff;
        uint64_t prev = text_fileoff;
        return raw_offsets(macho) |
               ranges::views::transform([&prev, text_fileoff](const auto raw_off) {
                   if (prev != text_fileoff) {
                       prev += raw_off;
                   }
                   return prev;
                   // return 0;
               });
    }
#endif

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
