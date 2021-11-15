#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuSubType.h"
#include "jevmachopp/CpuType.h"

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
            ctx.out(), "<CpuTypeMeta type: {:s} ({:#08x}) subtype: {:s} ({:#08x})>",
            magic_enum::enum_name(metatype.type), to_underlying_int(metatype.type),
            magic_enum::enum_name(metatype.subtype), to_underlying_int(metatype.subtype));
    }
};
