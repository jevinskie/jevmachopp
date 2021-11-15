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

LoadSubCommand* LoadCommand::subcmd() const {
    return (LoadSubCommand *)(this + 1);
}
