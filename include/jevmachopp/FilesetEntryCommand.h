#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"

class FilesetEntryCommand : public LoadSubCommand {
public:
    const char *name() const;
    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint64_t vmaddr;
    uint64_t fileoff;
    uint32_t name_offset;
    uint32_t reserved;

public:
    FilesetEntryCommand(const FilesetEntryCommand &) = delete;
    void operator=(const FilesetEntryCommand &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same_minus_header(FilesetEntryCommand, struct fileset_entry_command,
                                     struct load_command);
#endif
static_assert_size_is(FilesetEntryCommand, 24);

template <> struct fmt::formatter<FilesetEntryCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(FilesetEntryCommand const &filesetEntryCmd, FormatContext &ctx) {
        auto out = ctx.out();
        return filesetEntryCmd.format_to(out);
    }
};
