#include "grammar/identifier.hpp"
#include "grammar/expr.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar identifier ``") {
  std::istringstream input(""s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `a`") {
  std::istringstream input("a"s);
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `hello_world`") {
  std::istringstream input("hello_world"s);
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `;hello_world`") {
  std::istringstream input(";hello_world"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `\\0hello_world`") {
  std::istringstream input("\0hello_world"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `\\xf3\\x0c\\x00\\x00`") {
  std::istringstream input("\xf3\x0c\x00\x00"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar identifier `raise`") {
  std::istringstream input("raise"s);
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::AtomName<0>>(
      Input(input, "<unit>")));
}
