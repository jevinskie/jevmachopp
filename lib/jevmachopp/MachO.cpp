//
//  MachO.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/23/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/MachO.h"
#include <algorithm>
#include <assert.h>
#include <mach-o/loader.h>
#include <numeric>

uint64_t MachO::size() const {
    return sizeof(struct mach_header_64) + loadCommandSize();
}

uint64_t MachO::pack(uint8_t *buf, uint8_t *base) const {
    struct mach_header_64 *mh = (struct mach_header_64 *)buf;
    mh->magic = MH_MAGIC;
    mh->cputype = (int32_t)cputype;
    mh->cpusubtype = (int32_t)cpusubtype;
    mh->filetype = filetype;
    mh->flags = flags;
    mh->ncmds = (uint32_t)loadCommands.size();
    mh->sizeofcmds = (uint32_t)loadCommandSize();
    buf += sizeof(struct mach_header_64);
    for (auto &lc : loadCommands) {
        buf += lc.pack(buf, base);
    }
    return size();
}

uint64_t MachO::unpack(uint8_t *buf, uint8_t *base) {
    struct mach_header_64 *mh = (struct mach_header_64 *)buf;
    assert(mh->magic == MH_MAGIC);
    cputype = (CpuType)mh->cputype;
    cpusubtype = (CpuSubType)mh->cpusubtype;
    filetype = mh->filetype;
    flags = mh->flags;
    buf += sizeof(struct mach_header_64);
    for (int i = 0; i < mh->ncmds; i++) {
        LoadCommand cmd;
        buf += cmd.unpack(buf, base);
        loadCommands.push_back(std::move(cmd));
    }
    return size();
}

uint64_t MachO::loadCommandSize() const {
    uint64_t sz = 0;
    for (auto &lc : loadCommands)
        sz += lc.size();
    return sz;
}
