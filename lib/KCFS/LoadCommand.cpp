//
//  LoadCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/27/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "KCFS/LoadCommand.h"
#include <mach-o/loader.h>
#include <assert.h>
#include <memory>

#include "KCFS/SegmentCommand.h"
#include "KCFS/UUIDCommand.h"
#include "KCFS/EncryptionInfoCommand.h"
#include "KCFS/GenericCommand.h"

uint64_t LoadCommand::size() const {
  return sizeof(struct load_command) + subcmd->size();
}

uint64_t LoadCommand::pack(uint8_t *buf, uint8_t *base) const {
  struct load_command *lc = (struct load_command *)buf;
  lc->cmd = (uint32_t)cmd;
  lc->cmdsize = (uint32_t)size();
  buf += sizeof(struct load_command);
  subcmd->pack(buf, base);
  return size();
}

uint64_t LoadCommand::unpack(uint8_t *buf, uint8_t *base) {
  struct load_command *lc = (struct load_command *)buf;
  cmd = (LoadCommandType)lc->cmd;
  switch (cmd) {
    case LoadCommandType::SEGMENT:
      subcmd = std::make_unique<SegmentCommand>();
      break;
    case LoadCommandType::UUID:
      subcmd = std::make_unique<UUIDCommand>();
      break;
    case LoadCommandType::ENCRYPTION_INFO:
      subcmd = std::make_unique<EncryptionInfoCommand>();
      break;
    default:
      subcmd = std::make_unique<GenericCommand>();
      break;
  }
  subcmd->unpack(buf + sizeof(struct load_command), base);
  return size();
}
