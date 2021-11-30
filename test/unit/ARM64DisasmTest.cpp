#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>

#include <jevmachopp/ARM64Disasm.h>

using namespace ARM64Disasm;

TEST_CASE("BL", "[ARM64Disasm]") {
    REQUIRE(isBL(0x94002A57));
    REQUIRE_FALSE(isBL(0x93002A57));
}
