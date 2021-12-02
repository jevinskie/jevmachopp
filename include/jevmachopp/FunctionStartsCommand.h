#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"
#include "jevmachopp/LEB128.h"
#include "jevmachopp/LinkeditDataCommand.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/MachO.h"
#include "jevmachopp/Ranges.h"
#include "jevmachopp/SegmentCommand.h"

#include <nanorange/algorithm/find.hpp>

class FunctionStartsCommand : public LinkeditDataCommand {
public:
    raw_func_start_range raw_offsets(const MachO &macho) const;
    FuncStartIterator raw_offsets_cbegin(const MachO &macho) const;
    FuncStartIterator raw_offsets_cend() const;

    auto file_offsets(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        setIfNullAsserting(textSeg, [&]() {
            return macho.textSeg();
        });
        return raw_offsets(macho) | view::psum(textSeg->fileoff);
    }

    static uint64_t func_start_for_offsets(uint64_t off, auto offs) {
        const auto oofs_end = offs.end();
        if (offs.begin() == offs.end()) {
            return 0;
        }
        if (off < offs.front()) {
            return 0;
        }
        uint64_t last_off_seen;
        auto following_func = ranges::find_if(offs, [off, &last_off_seen](const uint64_t curr_off) {
            last_off_seen = curr_off;
            return curr_off > off;
        });
        if (following_func == oofs_end) {
            return last_off_seen;
        }
        const auto our_func = ranges::prev(following_func);
        return *our_func;
    }

    uint64_t func_start_for_file_offset(uint64_t fileoff, const MachO &macho,
                                        const SegmentCommand *textSeg = nullptr) const {
        return func_start_for_offsets(fileoff, file_offsets(macho, textSeg));
    }

    auto vm_addrs(const MachO &macho, const SegmentCommand *textSeg = nullptr) const {
        setIfNullAsserting(textSeg, [&]() {
            return macho.textSeg();
        });
        return raw_offsets(macho) | view::psum(textSeg->vmaddr_fileoff_delta());
    }

    uint64_t func_start_for_vm_addr(uint64_t vm_addr, const MachO &macho,
                                    const SegmentCommand *textSeg = nullptr) const {
        return func_start_for_offsets(vm_addr, vm_addrs(macho, textSeg));
    }

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    // static func_start_for_offset();

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
