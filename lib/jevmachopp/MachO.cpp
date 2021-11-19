#include "jevmachopp/MachO.h"
#include "jevmachopp/DySymtabCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/Strtab.h"
#include "jevmachopp/SymtabCommand.h"

#pragma mark MachO header

bool MachO::isMagicGood() const {
    return magic == MH_MAGIC_64;
}

#pragma mark load commands

LoadCommand::Iterator MachO::lc_cbegin() const {
    return LoadCommand::Iterator((const LoadCommand *)(this + 1));
}

LoadCommand::Iterator MachO::lc_cend() const {
    return LoadCommand::Iterator((const LoadCommand *)((uintptr_t)(this + 1) + lc_sizeof()));
}

size_t MachO::lc_size() const {
    return ncmds;
}

size_t MachO::lc_sizeof() const {
    return sizeofcmds;
}

MachO::lc_range MachO::loadCommands() const {
    return {lc_cbegin(), lc_cend()};
}

#pragma mark segments

ranges::any_view<const LoadCommand &> MachO::segmentLoadCommands() const {
    return ranges::views::filter(loadCommands(), [](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::SEGMENT_64;
    });
}

ranges::any_view<const SegmentCommand &> MachO::segments() const {
    return ranges::views::transform(
        segmentLoadCommands(), [](const LoadCommand &segLC) -> const SegmentCommand & {
            return *std::get<const SegmentCommand *>(segLC.subcmd()->get());
        });
}

const SegmentCommand *MachO::segmentWithName(const std::string_view &name) const {
    return ranges::find_if_or_nullptr(segments(), [=](const SegmentCommand &segCmd) {
        return segCmd.segName() == name;
    });
}

const SegmentCommand *MachO::textSeg() const {
    return segmentWithName("__TEXT");
}

const SegmentCommand *MachO::dataConstSeg() const {
    return segmentWithName("__DATA_CONST");
}

const SegmentCommand *MachO::dataSeg() const {
    return segmentWithName("__DATA");
}

const SegmentCommand *MachO::linkeditSeg() const {
    return segmentWithName("__LINKEDIT");
}

#pragma mark symtab

const SymtabCommand *MachO::symtab() const {
    const auto *lc = ranges::find_if_or_nullptr(loadCommands(), [=](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::SYMTAB;
    });
    if (!lc) {
        return nullptr;
    }
    return (const SymtabCommand *)lc->subcmd();
}

std::span<const NList> MachO::symtab_nlists() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return {};
    }
    return symtab_ptr->nlists(*this);
}

ranges::any_view<const char *> MachO::symtab_strtab_entries() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return {};
    }
    return ranges::views::transform(symtab_ptr->strtab_entries(*this),
                                    [](const auto &strchr) -> const char * {
                                        return &strchr;
                                    });
}

const char *MachO::strtab_data() const {
    const auto *symtab_ptr = symtab();
    if (!symtab_ptr) {
        return nullptr;
    }
    return symtab_ptr->strtab_data(*this);
}

#pragma mark dysymtab

const DySymtabCommand *MachO::dysymtab() const {
    const auto *lc = ranges::find_if_or_nullptr(loadCommands(), [=](const LoadCommand &lc) {
        return lc.cmd == LoadCommandType::DYSYMTAB;
    });
    if (!lc) {
        return nullptr;
    }
    return (const DySymtabCommand *)lc->subcmd();
}

std::span<const NList> MachO::local_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->ilocalsym],
            &nlists[dysymtab_ptr->ilocalsym + dysymtab_ptr->nlocalsym]};
}

size_t MachO::local_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nlocalsym;
}

std::span<const NList> MachO::ext_def_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->iextdefsym],
            &nlists[dysymtab_ptr->iextdefsym + dysymtab_ptr->nextdefsym]};
}

size_t MachO::ext_def_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nextdefsym;
}

std::span<const NList> MachO::undef_syms() const {
    const auto *symtab_ptr = symtab();
    const auto *dysymtab_ptr = dysymtab();
    if (!symtab_ptr || !dysymtab_ptr) {
        return {};
    }
    const auto nlists = symtab_ptr->nlists(*this);
    return {&nlists[dysymtab_ptr->iundefsym],
            &nlists[dysymtab_ptr->iundefsym + dysymtab_ptr->nundefsym]};
}

size_t MachO::undef_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nundefsym;
}

std::span<const uint32_t> MachO::indirect_syms_idxes(const DySymtabCommand *dysymtab_ptr) const {
    if (!dysymtab_ptr) {
        dysymtab_ptr = dysymtab();
        if (!dysymtab_ptr) {
            return {};
        }
    }
    return {(const uint32_t *)((uintptr_t)this + dysymtab_ptr->indirectsymoff),
            dysymtab_ptr->nindirectsyms};
}

template <typename R, typename... Args> struct FunctionTraitsBase {
    using RetType = R;
    using ArgTypes = std::tuple<Args...>;
    static constexpr std::size_t ArgCount = sizeof...(Args);
};

template <typename F> struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)> : FunctionTraitsBase<R, Args...> {
    using base = FunctionTraitsBase<R, Args...>;
    using Pointer = R (*)(Args...);
    using ArgTypes = std::tuple<Args...>;
    static constexpr std::size_t ArgCount = sizeof...(Args);
    template <std::size_t N> using NthArg = std::tuple_element_t<N, ArgTypes>;
    using FirstArg = NthArg<0>;
    using LastArg = NthArg<ArgCount - 1>;
};

template <typename R> struct FunctionTraits<R (*)()> : FunctionTraitsBase<R> {
    using Pointer = R (*)();
};

#define MKMEM(memFknPtr, object)                                                                   \
    (delegate<std::remove_reference_t<memFknPtr>>::make<std::remove_reference_t<decltype(object)>, \
                                                        memFknPtr>(object))
#include <memory>

ranges::any_view<const NList &> MachO::indirect_syms(const SymtabCommand *symtab_ptr,
                                                     const DySymtabCommand *dysymtab_ptr) const {
    // least rescanning, but ugly
    // symtab_ptr = set_if_null(symtab_ptr, symtab);
    // symtab_ptr = set_if_null_w_checked_ret(symtab_ptr, symtab, {});

#if 1
    const SymtabCommand (*dummy_fptr)() = nullptr;
    fmt::print("dummy_fptr type: {}\n", type_name<decltype(dummy_fptr)>());
    decltype(&MachO::symtab) ugh = nullptr;
    fmt::print("ugh type: {}\n", type_name<decltype(ugh)>());
    std::remove_cv_t<decltype(ugh)> ugh2 = nullptr;
    fmt::print("ugh2 type: {}\n", type_name<decltype(ugh2)>());
    // const SymtabCommand *() const
    //    remove_member_pointer<decltype(&MachO::symtab)>::member_type *oww = nullptr;
    //    fmt::print("oww type: {}\n", type_name<decltype(oww)>());
    boost::callable_traits::remove_member_cv_t<
        remove_member_pointer<decltype(&MachO::symtab)>::member_type> *oww2 = nullptr;
    fmt::print("oww2 type: {}\n", type_name<decltype(oww2)>());
    boost::callable_traits::remove_member_cv_t<
        remove_member_pointer<decltype(&MachO::symtab)>::member_type> *oww3 = nullptr;
    fmt::print("oww3 type: {}\n", type_name<decltype(oww3)>());
    const auto wtf = &MachO::symtab;
    const auto xyz = *(void **)&wtf;
    const auto &foo = &MachO::symtab;
    const auto bar = (std::remove_pointer_t<decltype(wtf)>)(wtf);
    const auto buzz = &std::remove_pointer_t<decltype(this)>::symtab;
    //    const auto bar = *(void**)&foo;
    fmt::print("foo type: {}\n", type_name<decltype(foo)>());
    fmt::print("bar type: {}\n", type_name<decltype(foo)>());
    fmt::print("test type: {}\n", type_name<std::remove_reference_t<decltype(foo)>>());
    fmt::print("foo: {:p}\n", (const void *)&foo);
#endif
    //    auto del2 =
    //    delegate<FunctionTraits<decltype(MachO::symtab)>::Pointer>::make<&MachO::symtab>(*this);
    //    auto del = delegate<const SymtabCommand *()>::make<&MachO::symtab>(*this);
    if (!setIfNull(symtab_ptr, DELEGATE_MKMEM2(&MachO::symtab, *this))) {
        return {};
    }
    // setIfNullErroringRet(symtab_ptr, delegate<const SymtabCommand
    // *()>::make<&MachO::symtab>(*this),
    //                      {});
    setIfNullErroringRet(dysymtab_ptr,
                         DELEGATE_MKMEM2(&MachO::dysymtab, *this), {});
    const auto nlists = symtab_ptr->nlists(*this);
    return ranges::views::transform(indirect_syms_idxes(),
                                    [nlists](const int idx) -> const NList & {
                                        return nlists[idx];
                                    });
}

size_t MachO::indirect_syms_size() const {
    const auto *dysymtab_ptr = dysymtab();
    if (!dysymtab_ptr) {
        return 0;
    }
    return dysymtab_ptr->nindirectsyms;
}

fmt::appender &MachO::format_to(fmt::appender &out) const {
    fmt::format_to(
        out,
        "<MachO @ {:p} cputype: {} fileType: {:#010x} flags: {:#010x} ncmds: {:d} sizeofcmds: {:#x} "_cf,
        (void *)this, cputype, filetype, flags, ncmds, sizeofcmds);
    fmt::format_to(out, "{}"_cf, fmt::join(loadCommands(), ", "));
    fmt::format_to(out, ">"_cf);
    return out;
}
