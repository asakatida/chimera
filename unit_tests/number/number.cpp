#include "object/number/number.hpp"

#include <catch.hpp>

template <typename OStream>
OStream &operator<<(OStream &os,
                    const chimera::library::object::number::Number &number) {
  return number.debug(os);
}

TEST_CASE("number Number") {
  chimera::library::object::number::Number number{};
  auto other = chimera::library::object::number::Number(2);
  REQUIRE(number < other);
  number = other + other;
  REQUIRE(std::uint64_t(number) == 4);
  number = number * other + other;
  REQUIRE(std::uint64_t(number) == 10);
  number = other * number + number;
  REQUIRE(std::uint64_t(number) == 30);
  number = number - other;
  REQUIRE(std::uint64_t(number) == 28);
  number = number % chimera::library::object::number::Number(3);
  REQUIRE(std::uint64_t(number) == 1);
  number = number - chimera::library::object::number::Number(1);
  REQUIRE(std::uint64_t(number) == 0);
}

TEST_CASE("number Number division") {
  auto other = chimera::library::object::number::Number(2);
  auto huge = chimera::library::object::number::Number(8);
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
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = huge + eight;
  auto massive = huge + two;
  massive += two;
  massive += two;
  massive += two;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number addition huge b") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = eight + huge;
  auto massive = two + two;
  massive += two;
  massive += two;
  massive += huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number addition huge c") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = huge + eight;
  auto massive = two + two;
  massive += two;
  massive += two;
  massive += huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number division huge") {
  auto other = chimera::library::object::number::Number(2);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
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

TEST_CASE("number Number multiplication huge a") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = huge * eight;
  auto massive = huge * two;
  massive *= two;
  massive *= two;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number multiplication huge b") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = eight * huge;
  auto massive = two * two;
  massive *= two;
  massive *= huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number multiplication huge c") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = huge * eight;
  auto massive = two * two;
  massive *= two;
  massive *= huge;
  REQUIRE(massive.is_int());
  REQUIRE(massive == number);
}

TEST_CASE("number Number naturals") {
  auto one = chimera::library::object::number::Number(1);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
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
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
  auto number = huge * huge;
  auto massive = number * number;
  REQUIRE((massive / number) == number);
}

TEST_CASE("number Number subtraction huge") {
  auto two = chimera::library::object::number::Number(2);
  auto eight = chimera::library::object::number::Number(8);
  auto huge = chimera::library::object::number::Number(
      std::numeric_limits<std::uint64_t>::max());
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
