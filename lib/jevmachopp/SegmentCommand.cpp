//
//  SegmentCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/27/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "KCFS/SegmentCommand.h"
#include <mach-o/loader.h>

uint64_t SegmentCommand::size() const {
  return sizeof(struct segment_command) - sizeof(struct load_command) +
         sizeof(struct section) * sections.size();
}

uint64_t SegmentCommand::pack(uint8_t *buf, uint8_t *base) const {
  buf -= sizeof(load_command);
  struct segment_command *seg = (struct segment_command *)buf;
  memcpy(seg->segname, segname, sizeof(segname));
  seg->vmaddr = vmaddr;
  seg->vmsize = vmsize;
  seg->maxprot = maxprot;
  seg->initprot = initprot;
  seg->nsects = (uint32_t)sections.size();
  seg->flags = flags;
  buf += sizeof(struct segment_command);
  for (auto & sec : sections) {
    buf += sec.pack(buf, base);
  }
  return size();
}

uint64_t SegmentCommand::unpack(uint8_t *buf, uint8_t *base) {
  buf -= sizeof(load_command);
  struct segment_command *seg = (struct segment_command *)buf;
  memcpy(segname, seg->segname, sizeof(segname));
  vmaddr = seg->vmaddr;
  vmsize = seg->vmsize;
  maxprot = seg->maxprot;
  initprot = seg->initprot;
  flags = seg->flags;
  buf += sizeof(struct segment_command);
  for (int i = 0; i < seg->nsects; i++) {
    Section sec;
    buf += sec.unpack(buf, base);
    sections.push_back(std::move(sec));
  }
  return size();
}
