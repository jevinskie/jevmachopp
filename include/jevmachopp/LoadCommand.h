#pragma once

#include <memory>

#include "jevmachopp/Common.h"
#include "jevmachopp/LoadCommandType.h"
#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/Packer.h"

class LoadCommand : public Packer {
  public:
    uint64_t pack(uint8_t *buf, uint8_t *base) const;
    uint64_t unpack(uint8_t *buf, uint8_t *base);
    uint64_t size() const;

  public:
    LoadCommandType cmd;
    std::unique_ptr<LoadSubCommand> subcmd;
};

//static_assert_cond(sizeof(LoadCommand) == sizeof(struct load_command));

//static_assert(sizeof(LoadCommand) == sizeof(struct load_command), HEDLEY_STRINGIFY(sizeof));
static_assert_size_same(LoadCommand, struct load_command);
// constexpr bool static_assert_size_same_0 = check_size<LoadCommand, struct load_command>();
