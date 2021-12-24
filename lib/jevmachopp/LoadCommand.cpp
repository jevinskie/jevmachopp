#include "jevmachopp/LoadCommand.h"

class LoadSubCommand;

const LoadSubCommand *LoadCommand::subcmd() const {
    return (const LoadSubCommand *)(this + 1);
}
