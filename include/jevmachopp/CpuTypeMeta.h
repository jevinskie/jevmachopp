#pragma once

#include "fmt/core.h"
#include "jevmachopp/Common.h"
#include "jevmachopp/CpuSubType.h"
#include "jevmachopp/CpuType.h"
#include "magic_enum.hpp"

class CpuTypeMeta {
public:
    CpuType type;
    CpuSubType subtype;
};

static_assert_cond(sizeof(CpuTypeMeta) == sizeof(CpuType) + sizeof(CpuSubType));

template <> struct fmt::formatter<CpuTypeMeta> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(CpuTypeMeta const &metatype, FormatContext &ctx) {
        return fmt::format_to(
            ctx.out(), "<CpuTypeMeta type: {:s} ({:#010x}) subtype: {:s} ({:#010x})>",
            magic_enum::enum_name(metatype.type), as_unsigned(metatype.type),
            magic_enum::enum_name(metatype.subtype), as_unsigned(metatype.subtype));
    }
};
