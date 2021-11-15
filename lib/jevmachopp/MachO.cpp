//
//  MachO.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/23/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/MachO.h"

const LoadCommand *MachO::lc_cbegin() const {
    return (const LoadCommand *)(this + 1);
}

const LoadCommand *MachO::lc_cend() const {
    return (const LoadCommand *)((uintptr_t)(this + 1) + lc_sizeof());
}

size_t MachO::lc_size() const {
    return ncmds;
}

size_t MachO::lc_sizeof() const {
    return sizeofcmds;
}