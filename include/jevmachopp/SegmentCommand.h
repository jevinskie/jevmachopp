#pragma once

#include <list>
#include <memory>
#include <mach/vm_prot.h>

#include "KCFS/LoadSubCommand.h"
#include "KCFS/Section.h"

class SegmentCommand : public LoadSubCommand {
public:
  uint64_t pack(uint8_t *buf, uint8_t *base) const override;
  uint64_t unpack(uint8_t *buf, uint8_t *base) override;
  uint64_t size() const override;
  uint32_t numSections();

public:
  char segname[16];
  uint32_t vmaddr;
  uint32_t vmsize;
  vm_prot_t maxprot;
  vm_prot_t initprot;
  uint32_t flags;
  std::list<Section> sections;
};
