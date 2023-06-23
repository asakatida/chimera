#include "grammar/grammar.hpp"
#include "grammar/input.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace std::literals;
using Input = chimera::library::grammar::Input;

TEST_CASE("grammar expression ``") {
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Expr<0>>>(
          Input(""sv, "<unit>")),
      tao::pegtl::parse_error);
}

TEST_CASE("grammar expression `a`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Expr<0>, tao::pegtl::eof>>(
      Input("a"sv, "<unit>")));
}

TEST_CASE("grammar expression `hello_world`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Expr<0>, tao::pegtl::eof>>(
      Input("hello_world"sv, "<unit>")));
}

TEST_CASE("grammar expression `\\0hello_world`") {
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Expr<0>>>(
          Input("\0hello_world"sv, "<unit>")),
      tao::pegtl::parse_error);
}
