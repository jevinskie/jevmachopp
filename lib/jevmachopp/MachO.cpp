//
//  MachO.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/23/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/MachO.h"

LoadCommand::Iterator MachO::lc_cbegin() const {
    return LoadCommand::Iterator((const LoadCommand *)(this + 1));
}

LoadCommand::Iterator MachO::lc_cend() const {
    return LoadCommand::Iterator((const LoadCommand *)((uintptr_t)(this + 1) + lc_sizeof()));
}

size_t MachO::lc_size() const {
    return ncmds;
}

size_t MachO::lc_sizeof() const {
    return sizeofcmds;
}