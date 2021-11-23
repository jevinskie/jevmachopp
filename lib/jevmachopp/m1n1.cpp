#include "jevmachopp/Common.h"

#if M1N1

extern "C" __attribute__((noreturn)) void
__assert_func(const char *file, int line, const char *function, const char *assertion) {
    __assert_fail(assertion, file, line, function);
}

#endif
