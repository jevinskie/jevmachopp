#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CommonTypes.h"

#include <cassert>
#include <string>

class MachO;
class SymtabCommand;

constexpr uint8_t JEV_N_TYPE  = 0x0e;
constexpr uint8_t JEV_N_UNDF  = 0x00;
constexpr uint8_t JEV_NO_SECT = 0;

constexpr uint32_t JEV_INDIRECT_SYMBOL_LOCAL = 0x80000000;
constexpr uint32_t JEV_INDIRECT_SYMBOL_ABS   = 0x40000000;

class NList {
public:
    const char *name(const char *strtab_data) const;
    const char *name(const MachO &macho, const SymtabCommand &symtabCmd) const;
    const char *name(const MachO &macho) const;

    uint8_t dylibOrdinal() const;
    const char *dylibName(const dylib_names_map_t &map) const;
    std::string_view segmentName(const segment_names_map_t &map) const;
    std::string_view sectionName(const section_names_map_t &map) const;
#if USE_FMT
    fmt::appender &format_to(fmt::appender &out) const;
    fmt::appender &format_to(fmt::appender &out, const MachO &macho,
                             const dylib_names_map_t *map = nullptr) const;
#endif

public:
    uint32_t strx;
    uint8_t type;
    uint8_t sect;
    uint16_t desc;
    uint64_t value;

public:
    NList(const NList &) = delete;
    void operator=(const NList &) = delete;
};

#if __has_include(<mach-o/nlist.h>)
#include <mach-o/nlist.h>
static_assert_size_same(NList, struct nlist_64);
#endif
static_assert_size_is(NList, 16);

template <typename T> struct value_extractor {
    T operator()(T value) {
        return value;
    }

    template <typename U> [[noreturn]] T operator()(U) {
        assert(!"invalid type");
    }
};

#if USE_FMT
template <> struct fmt::formatter<NList> {
    // Presentation format: 'd' - dumb, 'f' - fancy, 'm' - mapped fancy
    char presentation          = 'd';
    int macho_arg_id           = -1;
    int dylib_names_map_arg_id = -1;

    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'd' || *it == 'f' || *it == 'm'))
            presentation = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}')
            assert(!"invalid format");

        if (presentation == 'f' || presentation == 'm') {
            macho_arg_id = ctx.next_arg_id();
        }
        if (presentation == 'm') {
            dylib_names_map_arg_id = ctx.next_arg_id();
        }

        // Return an iterator past the end of the parsed range:
        return it;
    }

    auto format(NList const &nlist, format_context &ctx) -> decltype(ctx.out()) {
        using handle_t       = fmt::basic_format_arg<format_context>::handle;
        using custom_value_t = detail::custom_value<format_context>;
        static_assert_size_same(handle_t, custom_value_t);
        auto out = ctx.out();
        if (macho_arg_id >= 0) {
            auto macho_arg          = ctx.arg(macho_arg_id);
            auto macho_arg_handle   = visit_format_arg(value_extractor<handle_t>(), macho_arg);
            auto macho_custom_value = (custom_value_t *)&macho_arg_handle;
            const auto &macho       = *(const MachO *)macho_custom_value->value;
            if (dylib_names_map_arg_id >= 0) {
                auto dylib_names_map_arg     = ctx.arg(dylib_names_map_arg_id);
                auto dylib_names_map_arg_ptr = (const dylib_names_map_t *)visit_format_arg(
                    value_extractor<const void *>(), dylib_names_map_arg);
                return nlist.format_to(out, macho, dylib_names_map_arg_ptr);
            } else {
                return nlist.format_to(out, macho);
            }
        }
        return nlist.format_to(out);
    }
};
#endif
