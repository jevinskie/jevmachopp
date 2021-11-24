#pragma once

#include <boost/static_string/static_string.hpp>

template <typename Buf, std::size_t BufSz = sizeof(Buf)>
boost::static_string<BufSz> readMaybeNullTermCString(const char *cstr) {

    boost::static_string<512> buf2hexstr(const void *buf, const std::size_t buf_sz) {
        boost::static_string<buf_sz * 2> buf2;
        return buf2;
    }
