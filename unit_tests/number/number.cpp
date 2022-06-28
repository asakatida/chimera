#include "object/number/number.hpp"

#include <limits>

#include <catch2/catch.hpp>

using chimera::library::object::number::Number;

template <typename OStream>
auto operator<<(OStream &os, const Number &number) -> OStream & {
  return number.debug(os);
}

TEST_CASE("number Number") {
  Number number(0);
  Number other(2);
  REQUIRE(number < other);
  number = other + other;
  REQUIRE(std::uint64_t(number) == 4);
  number = number * other + other;
  REQUIRE(std::uint64_t(number) == 10);
  number = other * number + number;
  REQUIRE(std::uint64_t(number) == 30);
  number = number - other;
  REQUIRE(std::uint64_t(number) == 28);
  number = number % Number(3);
  REQUIRE(std::uint64_t(number) == 1);
  number = number - Number(1);
  REQUIRE(std::uint64_t(number) == 0);
}

TEST_CASE("number Number division", "[!shouldfail]") {
  Number other(2);
  Number huge(8);
  auto number = huge * huge;
  REQUIRE(number > huge);
  auto massive = number * other;
  REQUIRE(massive > number);
  REQUIRE(massive.is_int());
  massive = massive / huge;
  REQUIRE(massive > huge);
  REQUIRE(massive.is_int());
  auto test = huge * other;
  REQUIRE(massive == test);
  REQUIRE(massive.is_int());
  massive = massive / other;
  REQUIRE(massive == huge);
}

TEST_CASE("number Number addition huge a") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge + eight;
  auto massive = huge + two;
  massive += two;
  massive += two;
  massive += two;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number addition huge b") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = eight + huge;
  auto massive = two + two;
  massive += two;
  massive += two;
  massive += huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number addition huge c") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge + eight;
  auto massive = two + two;
  massive += two;
  massive += two;
  massive += huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number division huge") {
  Number other(2);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * huge;
  REQUIRE(number > huge);
  auto massive = number * other;
  REQUIRE(massive.is_int());
  REQUIRE(massive > number);
  massive = massive / huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive > huge);
  auto test = huge * other;
  REQUIRE(massive == test);
  massive = massive / other;
  REQUIRE(massive.is_int());
  REQUIRE(massive == huge);
}

TEST_CASE("number Number modulus huge") {
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * Number(3);
  REQUIRE((number % Number(2)) == Number(1));
}

TEST_CASE("number Number multiplication huge a") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * eight;
  auto massive = huge * two;
  massive *= two;
  massive *= two;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number multiplication huge b") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = eight * huge;
  auto massive = two * two;
  massive *= two;
  massive *= huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number multiplication huge c") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * eight;
  auto massive = two * two;
  massive *= two;
  massive *= huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number naturals") {
  Number one(1);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * huge;
  auto massive = number * number;
  REQUIRE(massive.is_int());
  REQUIRE(massive > number);
  REQUIRE((massive + huge) > massive);
  REQUIRE((massive + number) == (number * (number + one)));
  REQUIRE((massive - number) == (number * (number - one)));
  REQUIRE((massive * number) > massive);
  REQUIRE((massive / huge) == (number * huge));
  REQUIRE(std::uint64_t(massive & huge) == 1);
  REQUIRE((massive & number) > one);
}

TEST_CASE("number Number naturals costly", "[!hide]") {
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge * huge;
  auto massive = number * number;
  REQUIRE((massive / number) == number);
}

TEST_CASE("number Number subtraction huge") {
  Number two(2);
  Number eight(8);
  Number huge(std::numeric_limits<std::uint64_t>::max());
  auto number = huge + eight;
  auto massive = number;
  massive -= two;
  massive -= two;
  massive -= two;
  massive -= two;
  number -= eight;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
  REQUIRE(std::uint64_t(massive) == std::numeric_limits<std::uint64_t>::max());
  REQUIRE(std::uint64_t(number) == std::numeric_limits<std::uint64_t>::max());
}
