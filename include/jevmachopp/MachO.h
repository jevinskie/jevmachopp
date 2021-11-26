#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuTypeMeta.h"
#include "jevmachopp/DylibCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/MachOHeaderEnums.h"
#include "jevmachopp/PackedCStr.h"
#include "jevmachopp/SymtabCommand.h"

#include <array>
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <typeinfo>

#include <nanorange/views/filter.hpp>
#include <nanorange/views/subrange.hpp>
using namespace nano;

class DylibCommand;
class DySymtabCommand;
class SegmentCommand;
class SymtabCommand;
class NList;

using MachOMagic = std::uint32_t;

constexpr MachOMagic JEV_MH_MAGIC_64 = 0xfeedfacf;

class MachO {
public:
    bool isMagicGood() const;

    using lc_range = subrange<LoadCommand::Iterator>;
    lc_range loadCommands() const;
    LoadCommand::Iterator lc_cbegin() const;
    LoadCommand::Iterator lc_cend() const;
    std::size_t lc_size() const;
    std::size_t lc_sizeof() const;

#pragma mark segments
    auto segmentLoadCommands() const {
        return views::filter(loadCommands(), [](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::SEGMENT_64;
        });
    }
    auto segments() const {
        return views::transform(segmentLoadCommands(),
                                [](const LoadCommand &segLC) -> const SegmentCommand & {
                                    return *(const SegmentCommand *)segLC.subcmd();
                                });
    }
    const SegmentCommand *segmentWithName(const std::string_view &name) const;
    const SegmentCommand *textSeg() const;
    const SegmentCommand *dataConstSeg() const;
    const SegmentCommand *dataSeg() const;
    const SegmentCommand *linkeditSeg() const;

    AddrRange vmaddr_range() const;
    AddrRange file_range() const;

#pragma mark symtab
    const SymtabCommand *symtab() const;
    std::span<const NList> symtab_nlists() const;
    auto symtab_strtab_entries(const SymtabCommand *symtab_ptr = nullptr) const {
        setIfNull(symtab_ptr, [this]() {
            return symtab();
        });
        return ranges::views::transform(symtab_ptr ? symtab_ptr->strtab_entries(*this)
                                                   : packed_cstr_range{},
                                        [](const auto &strchr) -> const char * {
                                            return &strchr;
                                        });
    }
    const char *strtab_data() const;

#pragma mark dysymtab
    const DySymtabCommand *dysymtab() const;
    std::span<const NList> local_syms() const;
    std::size_t local_syms_size() const;
    std::span<const NList> ext_def_syms() const;
    std::size_t ext_def_syms_size() const;
    std::span<const NList> undef_syms() const;
    std::size_t undef_syms_size() const;
    indirect_syms_idxes_t indirect_syms_idxes(const DySymtabCommand *dysymtab_ptr = nullptr) const;
    auto indirect_syms(const SymtabCommand *symtab_ptr = nullptr,
                       const DySymtabCommand *dysymtab_ptr = nullptr) const {
        setIfNull(symtab_ptr, [this]() {
            return symtab();
        });
        setIfNull(dysymtab_ptr, [this]() {
            return dysymtab();
        });

        std::span<const NList> nlists = {};
        if (symtab_ptr) {
            nlists = symtab_ptr->nlists(*this);
        }
        return ranges::views::transform(dysymtab_ptr ? indirect_syms_idxes(dysymtab_ptr)
                                                     : indirect_syms_idxes_t{},
                                        [nlists](const int idx) -> const NList & {
                                            return nlists[idx];
                                        });
    }
    std::size_t indirect_syms_size() const;

#pragma mark dylibs
    auto dylibLoadCommands() const {
        return ranges::views::filter(loadCommands(), [](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::ID_DYLIB || lc.cmd == LoadCommandType::LOAD_DYLIB ||
                   lc.cmd == LoadCommandType::LOAD_WEAK_DYLIB ||
                   lc.cmd == LoadCommandType::REEXPORT_DYLIB;
        });
    }
    auto dylibCommands() const {
        return ranges::views::transform(dylibLoadCommands(),
                                        [](const LoadCommand &lc) -> const DylibCommand & {
                                            return *(const DylibCommand *)lc.subcmd();
                                        });
    }
    auto importedDylibLoadCommands() const {
        return ranges::views::filter(loadCommands(), [](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::LOAD_DYLIB ||
                   lc.cmd == LoadCommandType::LOAD_WEAK_DYLIB;
        });
    }
    auto importedDylibCommands() const {
        return ranges::views::transform(importedDylibLoadCommands(),
                                        [](const LoadCommand &lc) -> const DylibCommand & {
                                            return *(const DylibCommand *)lc.subcmd();
                                        });
    }
    dylib_names_map_t dylibNamesMap() const;

#pragma mark filesets
    auto filesetEntryLoadCommands() const {
        return ranges::views::filter(loadCommands(), [](const LoadCommand &lc) {
            return lc.cmd == LoadCommandType::FILESET_ENTRY;
        });
    }

    auto filesetEntryCommands() const {
        return views::transform(filesetEntryLoadCommands(),
                                [](const LoadCommand &fseLC) -> const FilesetEntryCommand & {
                                    return *(const FilesetEntryCommand *)fseLC.subcmd();
                                });
    }

#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
#endif

public:
    MachOMagic magic;
    CpuTypeMeta cputype;
    MachOFileType filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    MachOFlags flags;
    uint32_t reserved;

public:
    MachO(const MachO &) = delete;
    void operator=(const MachO &) = delete;
};

#if __has_include(<mach-o/loader.h>)
#include <mach-o/loader.h>
static_assert_size_same(MachO, struct mach_header_64);
#endif
static_assert_size_is(MachO, 32);

#if USE_FMT
template <> struct fmt::formatter<MachO> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(MachO const &macho, FormatContext &ctx) {
        auto out = ctx.out();
        return macho.format_to(out);
    }
};
#endif
