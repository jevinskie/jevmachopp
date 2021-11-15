#pragma once

#include "jevmachopp/Common.h"

class LoadCommand;

class LoadSubCommand {
public:
	const LoadCommand *loadCommand() const;
};

static_assert_size_is(LoadSubCommand, 1);
