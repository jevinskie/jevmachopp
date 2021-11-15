//
//  LoadCommand.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/27/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/LoadCommand.h"

class LoadSubCommand;

const LoadSubCommand *LoadCommand::subcmd() const {
    return (const LoadSubCommand *)(this + 1);
}
