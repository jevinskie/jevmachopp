#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/Atomic.h>

TEST_CASE("ctor std::atomic<std::pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<std::pair<uint64_t, uint64_t>> p;
}

TEST_CASE("load std::atomic<std::pair<uint64_t, uint64_t>>", "[atomic]") {
    std::atomic<std::pair<uint64_t, uint64_t>> p;
    p.load();
}
