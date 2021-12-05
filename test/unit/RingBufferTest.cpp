#include <cstdint>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

constexpr std::size_t NUM_ELEM = 0x1000;

TEST_CASE("ctor", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    SECTION("mirror [0]") {
        rb.m_buf[0] = 0xdeadbeef;
        REQUIRE(rb.m_buf_mirror[0] == rb.m_buf[0]);
    }
}

TEST_CASE("push 'n pop", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    SECTION("push") {
        rb.push(0xdeadbeef);
        REQUIRE(rb.peek() == 0xdeadbeef);
    }

    SECTION("pop") {
        rb.push(0xdeadbeef);
        REQUIRE(rb.pop() == 0xdeadbeef);
    }

    SECTION("2x size") {
        for (std::size_t i = 0; i < 1.5 * 2 * NUM_ELEM; ++i) {
            rb.push(i);
            REQUIRE(rb.pop() == i);
        }
    }
}

TEST_CASE("no overflow", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    for (std::size_t i = 0; i < rb.static_size; ++i) {
        rb.push(i);
    }
    SUCCEED("no overflow point");
}

#if 0
TEST_CASE("overflow by one", "[ringbuffer]") {
    auto rb = RingBuffer<uint32_t, NUM_ELEM>{};

    for (std::size_t i = 0; i < rb.static_size; ++i) {
        rb.push(i);
    }
    SUCCEED("no overflow point");
    rb.push(0xdeadbeef);
    REQUIRE_FALSE(true);
}
#endif
