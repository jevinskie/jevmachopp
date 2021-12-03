#pragma once

#include "jevmachopp/Common.h"

namespace SearchFS {

bool files_larger_than(const char *volume_path, std::size_t min_sz);

} // namespace SearchFS
