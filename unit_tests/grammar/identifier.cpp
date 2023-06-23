#include "grammar/identifier.hpp"
#include "grammar/expr.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar identifier ``") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(""sv, "<unit>")));
}

TEST_CASE("grammar identifier `a`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input("a"sv, "<unit>")));
}

TEST_CASE("grammar identifier `hello_world`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input("hello_world"sv, "<unit>")));
}

TEST_CASE("grammar identifier `;hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(";hello_world"sv, "<unit>")));
}

TEST_CASE("grammar identifier `\\0hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input("\0hello_world"sv, "<unit>")));
}

TEST_CASE("grammar identifier `\\xf3\\x0c\\x00\\x00`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input("\xf3\x0c\x00\x00"sv, "<unit>")));
}

TEST_CASE("grammar identifier `raise`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input("raise"s, "<unit>")));
}
