#pragma once

#include <array>
#include <mach/machine.h>
#include <memory>
#include <span>
#include <stdint.h>
#include <string>
#include <typeinfo>

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/LoadCommand.h"

#include <nanorange/views/filter.hpp>
#include <nanorange/views/subrange.hpp>
using namespace nano;

class DylibCommand;
class DySymtabCommand;
class SegmentCommand;
class SymtabCommand;
class NList;
class StrtabIterator;

using dylib_names_map_t = std::array<const char *, 0xFF>;

class MachO {
public:
    bool isMagicGood() const;

    using lc_range = ranges::subrange<LoadCommand::Iterator>;
    lc_range loadCommands() const;
    LoadCommand::Iterator lc_cbegin() const;
    LoadCommand::Iterator lc_cend() const;
    size_t lc_size() const;
    size_t lc_sizeof() const;

    int segmentLoadCommands() const;
    int segments() const;
    const SegmentCommand *segmentWithName(const std::string_view &name) const;
    const SegmentCommand *textSeg() const;
    const SegmentCommand *dataConstSeg() const;
    const SegmentCommand *dataSeg() const;
    const SegmentCommand *linkeditSeg() const;

    const SymtabCommand *symtab() const;
    std::span<const NList> symtab_nlists() const;
    std::ranges::subrange<const char *> symtab_strtab_entries() const;
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
    std::ranges::common_view<const NList *>
    indirect_syms(const SymtabCommand *symtab_ptr = nullptr,
                  const DySymtabCommand *dysymtab_ptr = nullptr) const;
    size_t indirect_syms_size() const;

    std::ranges::subrange<const LoadCommand &> dylibLoadCommands() const;
    std::ranges::subrange<const DylibCommand &> dylibCommands() const;
    std::ranges::subrange<const LoadCommand &> importedDylibLoadCommands() const;
    std::ranges::subrange<const DylibCommand &> importedDylibCommands() const;
    dylib_names_map_t dylibNamesMap() const;

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
