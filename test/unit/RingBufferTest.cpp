#include <cstdint>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/RingBuffer.h>

TEST_CASE("RingBuffer()", "[ringbuffer]") {
    REQUIRE_NOTHROW(RingBuffer<uint32_t, 0x4000>{});
}
