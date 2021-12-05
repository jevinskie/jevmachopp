#include <cstdint>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

TEST_CASE("ctor", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, 0x4000>{};

    SECTION("mirror [0]") {
        rb.m_buf[0] = 0xdeadbeef;
        REQUIRE(rb.m_buf_mirror[0] == rb.m_buf[0]);
    }
}

TEST_CASE("push 'n pop", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, 0x4000>{};

    SECTION("push 0") {
        rb.push(0xdeadbeef);
        REQUIRE(*rb.rd_ptr == 0xdeadbeef);
    }

    SECTION("pop 0") {
        rb.push(0xdeadbeef);
        REQUIRE(rb.pop() == 0xdeadbeef);
    }
}
