//
//  UUIDCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 6/4/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "KCFS/UUIDCommand.h"
#include <mach-o/loader.h>
#include <string.h>

uint64_t UUIDCommand::size() const { return sizeof(uuid); }

uint64_t UUIDCommand::pack(uint8_t *buf, uint8_t *base) const {
  buf -= sizeof(struct load_command);
  struct uuid_command *cmd = (struct uuid_command *)buf;
  memcpy(cmd->uuid, uuid, sizeof(uuid));
  return size();
}

uint64_t UUIDCommand::unpack(uint8_t *buf, uint8_t *base) {
  buf -= sizeof(struct load_command);
  struct uuid_command *cmd = (struct uuid_command *)buf;
  memcpy(uuid, cmd->uuid, sizeof(uuid));
  return size();
}