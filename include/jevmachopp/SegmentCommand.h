#pragma once

#include <list>
#include <mach/vm_prot.h>
#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Section.h"

#include <mach-o/loader.h>

class SegmentCommand : public LoadSubCommand {
  public:
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    vm_prot_t maxprot;
    vm_prot_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

static_assert_size_same_minus_header(SegmentCommand, struct segment_command_64, struct load_command);
