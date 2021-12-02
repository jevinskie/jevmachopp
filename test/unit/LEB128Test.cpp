#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/LEB128.h>

static constexpr uint8_t golden_0_7[]{0x00, 0x07};
static constexpr uint8_t golden_127_7[]{0x7f, 0x07};
static constexpr uint8_t golden_128_7[]{0x80, 0x01, 0x07};
static constexpr uint8_t golden_128_128[]{0x80, 0x01, 0x80, 0x01};
static constexpr uint8_t golden_7_0[]{0x07, 0x00};

TEST_CASE("uleb128", "[leb128]") {
    uint64_t val;
    std::size_t nb;
    std::size_t idx;

    idx = 0;
    val = 0xdeadbeef;
    nb = bfs::DecodeUleb128(golden_0_7, &val);
    REQUIRE(nb == 1);
    REQUIRE(val == 0);
    idx += nb;
    val = 0xdeadbeef;
    nb = bfs::DecodeUleb128(make_span(golden_0_7).subspan(idx), &val);
    REQUIRE(nb == 1);
    REQUIRE(val == 7);
    idx += nb;
    val = 0xdeadbeef;
    nb = bfs::DecodeUleb128(make_span(golden_0_7).subspan(idx), &val);
    REQUIRE(nb == 0);
    REQUIRE(val == 0xdeadbeef);
}
