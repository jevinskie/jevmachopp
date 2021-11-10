#pragma once

#include <string>
#include <stdint.h>
#include <list>
#include <memory>
#include <mach/machine.h>

#include "KCFS/Packer.h"
#include "KCFS/LoadCommand.h"
#include "KCFS/CpuType.h"
#include "KCFS/CpuSubType.h"

class MachO : public Packer {
public:
  uint64_t pack(uint8_t *buf, uint8_t *base) const override;
  uint64_t unpack(uint8_t *buf, uint8_t *base) override;
  uint64_t size() const override;

public:
  CpuType cputype;
  CpuSubType cpusubtype;
  uint32_t filetype;
  uint32_t flags;
  std::list<LoadCommand> loadCommands;

private:
  uint64_t loadCommandSize() const;
};
