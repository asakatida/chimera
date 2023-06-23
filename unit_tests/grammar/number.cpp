#include "grammar/number.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar number ``") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(""sv, "<unit>")));
}

TEST_CASE("grammar number `a`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input("1"sv, "<unit>")));
}

TEST_CASE("grammar number `hello_world`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input("0.1"sv, "<unit>")));
}

TEST_CASE("grammar number `;hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input(";hello_world"sv, "<unit>")));
}

TEST_CASE("grammar number `\\0hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input("\0hello_world"sv, "<unit>")));
}

TEST_CASE("grammar number `\\xf3\\x0c\\x00\\x00`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      Input("\xf3\x0c\x00\x00"sv, "<unit>")));
}
