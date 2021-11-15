#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadSubCommand.h"

const LoadCommand *LoadSubCommand::loadCommand() const {
	return (const LoadCommand *)((uintptr_t)this - sizeof(LoadCommand));
}
