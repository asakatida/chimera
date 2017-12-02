#include "object/number/number.hpp"

#include <catch.hpp>

TEST_CASE("number Number") {
  chimera::library::object::number::Number number;
  auto other = chimera::library::object::number::number(2);
  REQUIRE(number < other);
  number = other + other;
  REQUIRE(number == chimera::library::object::number::number(4));
  number = number * other + other;
  REQUIRE(number == chimera::library::object::number::number(10));
  number = other * number + number;
  REQUIRE(number == chimera::library::object::number::number(30));
  number = number - other;
  REQUIRE(number == chimera::library::object::number::number(28));
  number = number % chimera::library::object::number::number(3);
  REQUIRE(number == chimera::library::object::number::number(1));
  number = number - chimera::library::object::number::number(1);
  REQUIRE(number == chimera::library::object::number::number(0));
  number = number * (other + other);
  REQUIRE(number == chimera::library::object::number::number(0));
  auto huge = chimera::library::object::number::number(
      std::numeric_limits<std::uint64_t>::max());
  number = huge * huge;
  REQUIRE(number > huge);
}
