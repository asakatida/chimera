#include "grammar/grammar.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar expression ``") {
  std::stringstream input(""s);
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Expr<0>>>(
          Input(input, "<unit>")),
      tao::pegtl::parse_error);
}

TEST_CASE("grammar expression `a`") {
  std::stringstream input("a"s);
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Expr<0>, tao::pegtl::eof>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar expression `hello_world`") {
  std::stringstream input("hello_world"s);
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Expr<0>, tao::pegtl::eof>>(
      Input(input, "<unit>")));
}

TEST_CASE("grammar expression `\\0hello_world`") {
  std::stringstream input("\0hello_world"s);
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Expr<0>>>(
          Input(input, "<unit>")),
      tao::pegtl::parse_error);
}
