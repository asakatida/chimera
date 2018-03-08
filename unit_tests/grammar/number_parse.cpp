#include "grammar/number.hpp"

#include <catch.hpp>

#include "grammar/test_parse.hpp"

using namespace std::literals;

TEST_CASE("grammar construct number (3.14)") {
  REQUIRE_NOTHROW(chimera::library::test_parse("3.14"s));
}

TEST_CASE("grammar construct number (314)") {
  REQUIRE_NOTHROW(chimera::library::test_parse("314"s));
}
