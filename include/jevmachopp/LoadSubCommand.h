#pragma once

#include "jevmachopp/Common.h"

#include <variant>

class LoadCommand;
class EncryptionInfoCommand;
class SegmentCommand;
class UUIDCommand;
class UnknownCommand;

using SubCommandVariant = std::variant<const EncryptionInfoCommand *, const SegmentCommand *,
                                       const UUIDCommand *, const UnknownCommand *>;

class LoadSubCommand {
public:
    const LoadCommand *loadCommand() const;
    const SubCommandVariant get() const;
};

static_assert_size_is(LoadSubCommand, 1);
