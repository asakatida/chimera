#include "grammar/number.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar number ``") {
  std::istringstream input(""s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar number `1`") {
  std::istringstream input("1"s);
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar number `0.1`") {
  std::istringstream input("0.1"s);
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar number `;hello_world`") {
  std::istringstream input(";hello_world"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar number `\\0hello_world`") {
  std::istringstream input("\0hello_world"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar number `\\xf3\\x0c\\x00\\x00`") {
  std::istringstream input("\xf3\x0c\x00\x00"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(input, "<unit>")));
}
