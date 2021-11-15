#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadSubCommand.h"

const LoadCommand *LoadSubCommand::loadCommand() const {
	return (const LoadCommand *)((uintptr_t)this - sizeof(LoadCommand));
}

const SubCommandVariant LoadSubCommand::get() const {
	return SubCommandVariant{(const EncryptionInfoCommand*)nullptr};
}
