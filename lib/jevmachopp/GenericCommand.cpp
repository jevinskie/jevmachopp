//
//  GenericCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 6/4/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/GenericCommand.h"

#include <assert.h>
#include <mach-o/loader.h>
#include <string.h>

uint64_t GenericCommand::size() const {
    return data.size();
}

uint64_t GenericCommand::pack(uint8_t *buf, uint8_t *base) const {
    memcpy(buf, data.data(), data.size());
    return size();
}

uint64_t GenericCommand::unpack(uint8_t *buf, uint8_t *base) {
    buf -= sizeof(struct load_command);
    struct load_command *cmd = (struct load_command *)buf;
    data.insert(data.end(), buf + sizeof(struct load_command), buf + cmd->cmdsize);
    return size();
}