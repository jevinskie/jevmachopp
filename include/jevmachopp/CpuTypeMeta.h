#pragma once

#include "Common.h"
#include "CpuSubType.h"
#include "CpuType.h"
#include "Packer.h"

class CpuTypeMeta : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    CpuType type;
    CpuSubType subtype;
};

static_assert_cond(sizeof(CpuTypeMeta) == sizeof(CpuType) + sizeof(CpuSubType));

template <> struct fmt::formatter<CpuTypeMeta> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(CpuTypeMeta const &metatype, FormatContext &ctx) {
        return fmt::format_to(
            ctx.out(),
            "<CpuTypeMeta type: {:s} ({:#08x}) subtype: {:s} ({:#08x})>",
            magic_enum::enum_name(metatype.type),
            to_underlying_int(metatype.type),
            magic_enum::enum_name(metatype.subtype),
            to_underlying_int(metatype.subtype));
    }
};
