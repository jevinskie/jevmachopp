//
//  Section.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/27/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/Section.h"
#include <mach-o/loader.h>
#include <string.h>

uint64_t Section::size() const { return sizeof(section); }

uint64_t Section::pack(uint8_t *buf, uint8_t *base) const {
    struct section *s = (struct section *)buf;
    memcpy(s->sectname, sectname, sizeof(s->sectname));
    memcpy(s->segname, segname, sizeof(s->sectname));
    s->addr = addr;
    s->size = size_dumb;
    s->offset = offset;
    s->align = align;
    s->reloff = reloff;
    s->nreloc = nreloc;
    s->flags = flags;
    s->reserved1 = reserved1;
    s->reserved2 = reserved2;

    return size();
}

uint64_t Section::unpack(uint8_t *buf, uint8_t *base) {
    struct section *s = (struct section *)buf;
    memcpy(sectname, s->sectname, sizeof(sectname));
    memcpy(segname, s->segname, sizeof(segname));
    addr = s->addr;
    size_dumb = s->size;
    offset = s->offset;
    align = s->align;
    reloff = s->reloff;
    nreloc = s->nreloc;
    flags = s->flags;
    reserved1 = s->reserved1;
    reserved2 = s->reserved2;

    return size();
}
