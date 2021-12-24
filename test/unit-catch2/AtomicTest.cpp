#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/Atomic.h>
#include <jevmachopp/CommonTypes.h>

// STL pair not trivially copyable
#if 0
TEST_CASE("ctor std::atomic<std::pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<std::pair<uint64_t, uint64_t>> p;
}

TEST_CASE("load std::atomic<std::pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<std::pair<uint64_t, uint64_t>> p;
    p.load();
}
#endif

TEST_CASE("ctor std::atomic<con_pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<con_pair<uint64_t, uint64_t>> p;
}

TEST_CASE("load std::atomic<con_pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<con_pair<uint64_t, uint64_t>> p;
    p.load();
}

TEST_CASE("load int 128", "[atomic]") {
    std::atomic<uint128_t> lol;
    lol.store((uint128_t)0xdeadbeef'00000000ull << 64 | 0xbaadc0de'00000000ull);
    const auto res   = lol.load(std::memory_order_relaxed);
    const auto &pair = *(con_pair<uint64_t, uint64_t> *)&res;
    REQUIRE(pair.first == 0xbaadc0de'00000000ull);
    REQUIRE(pair.second == 0xdeadbeef'00000000ull);
}

TEST_CASE("load real con_pair u64", "[atomic]") {
    std::atomic<con_pair<uint64_t, uint64_t>> rofl;
    rofl.store({0xfacefeed, 0x00070007}, std::memory_order_relaxed);
    const auto res = rofl.load(std::memory_order_relaxed);
    REQUIRE(res.first == 0xfacefeed);
    REQUIRE(res.second == 0x00070007);
}

TEST_CASE("load operator= real con_pair u64", "[atomic]") {
    std::atomic<con_pair<uint64_t, uint64_t>> rofl;
    rofl.store({0xfacefeed, 0x00070007}, std::memory_order_relaxed);
    const decltype(rofl)::value_type res = rofl;
    REQUIRE(res.first == 0xfacefeed);
    REQUIRE(res.second == 0x00070007);
}
