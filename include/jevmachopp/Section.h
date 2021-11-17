#pragma once

#include <list>
#include <mach-o/loader.h>
#include <stdint.h>

#include "jevmachopp/Common.h"

class Section {
public:
    Section(const Section &) = delete;
    void operator=(const Section &) = delete;

public:
    std::string sectName() const;
    std::string segName() const;
    fmt::appender &format_to(fmt::appender &out) const;

public:
    char sectname[16];
    char segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
};

static_assert_size_same(Section, struct section_64);

template <> struct fmt::formatter<Section> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(Section const &sect, FormatContext &ctx) {
        auto out = ctx.out();
        return sect.format_to(out);
    }
};
