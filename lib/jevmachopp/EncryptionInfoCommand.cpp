//
//  EncryptionInfoCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 6/4/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/EncryptionInfoCommand.h"
#include <mach-o/loader.h>
#include <string.h>

uint64_t EncryptionInfoCommand::size() const {
    return sizeof(encryption_info_command);
}

uint64_t EncryptionInfoCommand::pack(uint8_t *buf, uint8_t *base) const {
    buf -= sizeof(struct load_command);
    struct encryption_info_command *cmd = (struct encryption_info_command *)buf;
    cmd->cryptsize = (uint32_t)data.size();
    cmd->cryptid = cryptid;
    // TODO: cryptoff memcpy
    return size();
}

uint64_t EncryptionInfoCommand::unpack(uint8_t *buf, uint8_t *base) {
    buf -= sizeof(struct load_command);
    struct encryption_info_command *cmd = (struct encryption_info_command *)buf;
    cryptid = cmd->cryptid;
    data.insert(data.end(), base + cmd->cryptoff,
                base + cmd->cryptoff + cmd->cryptsize);
    return size();
}