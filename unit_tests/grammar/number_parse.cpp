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

TEST_CASE("grammar construct number (3e3)") {
  REQUIRE_NOTHROW(chimera::library::test_parse("3e3"s));
}

TEST_CASE("grammar construct number (3e-3)", "[!hide]") {
  REQUIRE_NOTHROW(chimera::library::test_parse("3e-3"s));
}

TEST_CASE("grammar construct number (3e-333503335)", "[!hide]") {
  REQUIRE_NOTHROW(chimera::library::test_parse("3e-333503335"s));
}
