#pragma once

#include "jevmachopp/Common.h"
#include "jevmachopp/CpuSubType.h"
#include "jevmachopp/CpuType.h"

class CpuTypeMeta {
public:
    CpuTypeMeta(const CpuTypeMeta &) = delete;
    void operator=(const CpuTypeMeta &) = delete;

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
        return fmt::format_to(ctx.out(),
                              "<CpuTypeMeta type: {:s} ({:#010x}) subtype: ({:#010x})>"_cf,
                              CpuType_traits::to_string_or_empty(metatype.type),
                              as_unsigned(metatype.type), as_unsigned(metatype.subtype));
    }
};
