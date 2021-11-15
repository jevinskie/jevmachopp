//
//  LoadCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/27/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include <assert.h>
#include <mach-o/loader.h>
#include <memory>

#include "jevmachopp/EncryptionInfoCommand.h"
#include "jevmachopp/GenericCommand.h"
#include "jevmachopp/SegmentCommand.h"
#include "jevmachopp/UUIDCommand.h"
#include "jevmachopp/LoadCommand.h"

uint64_t LoadCommand::size() const {
    return cmdsize;
}

LoadSubCommand* LoadCommand::subcmd() const {
    return (LoadSubCommand *)(this + 1);
}

uint64_t LoadCommand::pack(uint8_t *buf, uint8_t *base) const {
    struct load_command *lc = (struct load_command *)buf;
    lc->cmd = (uint32_t)cmd;
    lc->cmdsize = (uint32_t)size();
    buf += sizeof(struct load_command);
    subcmd()->pack(buf, base);
    return size();
}

uint64_t LoadCommand::unpack(uint8_t *buf, uint8_t *base) {
    struct load_command *lc = (struct load_command *)buf;
    subcmd()->unpack(buf + sizeof(struct load_command), base);
    return size();
}
