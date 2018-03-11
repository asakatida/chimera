#include "object/number/number.hpp"

#include <catch.hpp>

TEST_CASE("number Number") {
  chimera::library::object::number::Number number;
  auto other = chimera::library::object::number::Number(2);
  REQUIRE(number < other);
  number = other + other;
  REQUIRE(number == chimera::library::object::number::Number(4));
  number = number * other + other;
  REQUIRE(number == chimera::library::object::number::Number(10));
  number = other * number + number;
  REQUIRE(number == chimera::library::object::number::Number(30));
  number = number - other;
  REQUIRE(number == chimera::library::object::number::Number(28));
  number = number % chimera::library::object::number::Number(3);
  REQUIRE(number == chimera::library::object::number::Number(1));
  number = number - chimera::library::object::number::Number(1);
  REQUIRE(number == chimera::library::object::number::Number());
  number = number * (other + other);
  REQUIRE(number == chimera::library::object::number::Number());
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  number = huge * huge;
  REQUIRE(number > huge);
  auto massive = number * other;
  REQUIRE(massive > number);
  REQUIRE(massive.is_int());
  massive = massive / huge;
  REQUIRE(massive > huge);
  REQUIRE(massive.is_int());
  massive = massive / other;
  REQUIRE(massive == huge);
}
