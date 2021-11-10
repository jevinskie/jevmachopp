#pragma once

#include "KCFS/Packer.h"
#include <list>

class Section : public Packer {
public:
  uint64_t pack(uint8_t *buf, uint8_t *base) const override;
  uint64_t unpack(uint8_t *buf, uint8_t *base) override;
  uint64_t size() const override;

public:
  char sectname[16];
  char segname[16];
  uint32_t addr;
  uint32_t size_dumb;
  uint32_t offset;
  uint32_t align;
  uint32_t reloff;
  uint32_t nreloc;
  uint32_t flags;
  uint32_t reserved1;
  uint32_t reserved2;
};
