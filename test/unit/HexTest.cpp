#include <fmt/core.h>
#include <catch2/catch_test_macros.hpp>

#include <jevmachopp/Hex.h>

TEST_CASE( "Integers are parsed from string_views", "[sv2int]" ) {
    REQUIRE( !sv2int<uint32_t>("lol"sv) );
}
