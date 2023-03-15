#include "grammar/grammar.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;
using Input = chimera::library::grammar::Input<tao::pegtl::string_input<>>;

TEST_CASE("grammar statement ``") {
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Stmt<0>>>(
          Input(""s, "<unit>")),
      tao::pegtl::parse_error);
}

TEST_CASE("grammar statement `a`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Stmt<0>, tao::pegtl::eof>>(
      Input("a"s, "<unit>")));
}

TEST_CASE("grammar statement `hello_world`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Stmt<0>, tao::pegtl::eof>>(
      Input("hello_world"s, "<unit>")));
}

TEST_CASE("grammar statement `;hello_world`") {
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Stmt<0>>>(
          Input(";hello_world"s, "<unit>")),
      tao::pegtl::parse_error);
}

TEST_CASE("grammar statement `hello_world;`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Stmt<0>, tao::pegtl::eof>>(
      Input("hello_world;"sv, "<unit>")));
}

TEST_CASE("grammar statement `\\0hello_world`") {
  REQUIRE_THROWS_AS(
      tao::pegtl::parse<tao::pegtl::must<chimera::library::grammar::Stmt<0>>>(
          Input("\0hello_world"s, "<unit>")),
      tao::pegtl::parse_error);
}

TEST_CASE("grammar statement `0=help`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Stmt<0>, tao::pegtl::eof>>(
      Input("0=help"s, "<unit>")));
}

TEST_CASE("grammar statement `raise`") {
  REQUIRE(tao::pegtl::parse<
          tao::pegtl::seq<chimera::library::grammar::Stmt<0>, tao::pegtl::eof>>(
      Input("raise"s, "<unit>")));
}
