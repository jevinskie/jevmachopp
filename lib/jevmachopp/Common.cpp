#include "jevmachopp/Common.h"

std::string_view readMaybeNullTermCString(const char *cstr, std::size_t buf_sz) {
    if (!cstr) {
        return {};
    }
    if (std::memchr(cstr, '\0', buf_sz)) {
        return {cstr};
    } else {
        return {cstr, buf_sz};
    }
}
