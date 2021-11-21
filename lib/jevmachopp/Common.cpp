#include "jevmachopp/Common.h"

namespace boost {

#if defined(BOOST_NO_EXCEPTIONS)

BOOST_NORETURN void throw_exception(std::exception const &e) {
    assert(!__PRETTY_FUNCTION__);
}

BOOST_NORETURN void throw_exception(std::exception const &e, boost::source_location const &loc) {
    assert(!__PRETTY_FUNCTION__);
}
#endif

} // namespace boost

extern "C"
void *_ZTVSt12length_error;

extern "C"
void _ZNSt11logic_errorC2EPKc(const void *thiz, const char *err) {
    assert(!__PRETTY_FUNCTION__);
}
