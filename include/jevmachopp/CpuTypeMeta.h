#pragma once

#include "Common.h"
#include "CpuType.h"
#include "CpuSubType.h"
#include "Packer.h"

class CpuTypeMeta : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    CpuType cputype;
    CpuSubType cpusubtype;
};

static_assert_cond(sizeof(CpuTypeMeta) == sizeof(CpuType) + sizeof(CpuSubType));
