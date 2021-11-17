#include "jevmachopp/Common.h"

#include <cstring>

std::string readMaybeNullTermCString(const char *cstr, size_t cstr_buf_sz) {
    if (!cstr)
        return "";
    if (std::memchr(cstr, '\0', cstr_buf_sz)) {
        return std::string{cstr};
    } else {
        return std::string{cstr, cstr_buf_sz};
    }
}
