#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/Hex.h>

TEST_CASE("not integer strings", "[sv2int]") {
    REQUIRE_FALSE(sv2int<uint32_t>("lol"sv));
    REQUIRE_FALSE(sv2int<uint32_t>("0x"sv));
}

TEST_CASE("zero", "[sv2int]") {
    REQUIRE(sv2int<uint32_t>("0"sv) == 0);
    REQUIRE(sv2int<uint32_t>("000"sv) == 0);
    REQUIRE(sv2int<uint32_t>("0x0"sv) == 0);
    REQUIRE(sv2int<uint32_t>("0x000"sv) == 0);
}

TEST_CASE("full width hex", "[sv2int]") {
    REQUIRE(sv2int<uint32_t>("0xdeadbeef"sv) == 0xdeadbeef);
    REQUIRE(sv2int<uint32_t>("0x12345678"sv) == 0x12345678);
    REQUIRE(sv2int<uint32_t>("0x21436587"sv) == 0x21436587);
    REQUIRE(sv2int<uint32_t>("0x87654321"sv) == 0x87654321);
    REQUIRE(sv2int<uint32_t>("0x78563412"sv) == 0x78563412);
}

TEST_CASE("ascending hex", "[sv2int]") {
    REQUIRE(sv2int<uint32_t>("0x0"sv) == 0x0);
    REQUIRE(sv2int<uint32_t>("0x10"sv) == 0x10);
    REQUIRE(sv2int<uint32_t>("0x210"sv) == 0x210);
    REQUIRE(sv2int<uint32_t>("0x3210"sv) == 0x3210);
    REQUIRE(sv2int<uint32_t>("0x1"sv) == 0x1);
    REQUIRE(sv2int<uint32_t>("0x21"sv) == 0x21);
    REQUIRE(sv2int<uint32_t>("0x321"sv) == 0x321);
    REQUIRE(sv2int<uint32_t>("0x4321"sv) == 0x4321);
}

// TEST_CASE("must be unsigned int", "[sv2int]") {
//     REQUIRE(sv2int<int32_t>("0x0"sv) == 0x0);
// }

// TEST_CASE("can't be bool", "[sv2int]") {
//     REQUIRE(sv2int<bool>("0x0"sv) == false);
// }
