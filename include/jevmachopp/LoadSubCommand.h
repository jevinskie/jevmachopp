#pragma once

#include "jevmachopp/Common.h"

class LoadCommand;

class LoadSubCommand {
public:
	const LoadCommand *loadCommand() const;
};
