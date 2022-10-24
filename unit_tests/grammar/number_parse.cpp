#include "grammar/number.hpp"

#include <catch2/catch_test_macros.hpp>

#include "grammar/test_parse.hpp"

using namespace std::literals;

TEST_CASE("grammar construct number (3.14)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("3.14"s),
                    chimera::library::object::AttributeError);
}

TEST_CASE("grammar construct number (314)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("314"s),
                    chimera::library::object::AttributeError);
}

TEST_CASE("grammar construct number (3e3)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("3e3"s),
                    chimera::library::object::AttributeError);
}

TEST_CASE("grammar construct number (3e-3)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("3e-3"s),
                    chimera::library::object::AttributeError);
}

TEST_CASE("grammar construct number (1j)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("1j"s),
                    chimera::library::object::AttributeError);
}

TEST_CASE("grammar construct number (3e-333503335)") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("3e-333503335"s),
                    chimera::library::object::AttributeError);
}
