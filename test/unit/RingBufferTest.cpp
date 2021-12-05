#include <cstdint>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

TEST_CASE("RingBuffer()", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, 0x4000>{};

    SECTION("mirror [0]") {
        rb.m_buf[0] = 0xdeadbeef;
        REQUIRE(rb.m_buf_mirror[0] == rb.m_buf[0]);
    }
}
