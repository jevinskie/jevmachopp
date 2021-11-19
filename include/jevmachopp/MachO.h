#pragma once

#include <list>
#include <mach/machine.h>
#include <memory>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/subrange.hpp>
#include <span>
#include <stdint.h>
#include <string>
#include <typeinfo>

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"

class SegmentCommand;
class SymtabCommand;
class DySymtabCommand;
class NList;
class StrtabIterator;

class MachO {
public:
    bool isMagicGood() const;

    using lc_range = ranges::subrange<LoadCommand::Iterator>;
    lc_range loadCommands() const;
    LoadCommand::Iterator lc_cbegin() const;
    LoadCommand::Iterator lc_cend() const;
    size_t lc_size() const;
    size_t lc_sizeof() const;

    ranges::any_view<const LoadCommand &> segmentLoadCommands() const;
    ranges::any_view<const SegmentCommand &> segments() const;
    const SegmentCommand *segmentWithName(const std::string_view &name) const;
    const SegmentCommand *textSeg() const;
    const SegmentCommand *dataConstSeg() const;
    const SegmentCommand *dataSeg() const;
    const SegmentCommand *linkeditSeg() const;

    const SymtabCommand *symtab() const;
    std::span<const NList> symtab_nlists() const;
    ranges::any_view<const char *> symtab_strtab_entries() const;
    const char *strtab_data() const;

    const DySymtabCommand *dysymtab() const;
    std::span<const NList> local_syms() const;
    size_t local_syms_size() const;
    std::span<const NList> ext_def_syms() const;
    size_t ext_def_syms_size() const;
    std::span<const NList> undef_syms() const;
    size_t undef_syms_size() const;
    std::span<const uint32_t>
    indirect_syms_idxes(const DySymtabCommand *dysymtab_ptr = nullptr) const;
    ranges::any_view<const NList &>
    indirect_syms(const SymtabCommand *symtab_ptr = nullptr,
                  const DySymtabCommand *dysymtab_ptr = nullptr) const;
    size_t indirect_syms_size() const;

    fmt::appender &format_to(fmt::appender &out) const;

public:
    uint32_t magic;
    CpuTypeMeta cputype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;

public:
    MachO(const MachO &) = delete;
    void operator=(const MachO &) = delete;
};

static_assert_size_same(MachO, struct mach_header_64);

template <> struct fmt::formatter<MachO> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(MachO const &macho, FormatContext &ctx) {
        auto out = ctx.out();
        return macho.format_to(out);
    }
};
