#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/ARM64Disasm.h>

using namespace ARM64Disasm;

TEST_CASE("isBL", "[ARM64Disasm]") {
    REQUIRE(isBL(0x94002a57));
    REQUIRE(isBL(0x97ffffff));
    REQUIRE_FALSE(isBL(0x93002a57));
}

TEST_CASE("decodeBL", "[ARM64Disasm]") {
    REQUIRE_FALSE(decodeBL(0x93002a57));
    REQUIRE(decodeBL(0x94002a57) == 0xa95c);
    REQUIRE(decodeBL(0x97ffffff) == -0x4);
}
