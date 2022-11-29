#include "grammar/number.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace std::literals;

TEST_CASE("grammar number ``") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>(""s, "<unit>")));
}

TEST_CASE("grammar number `a`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>("1"s, "<unit>")));
}

TEST_CASE("grammar number `hello_world`") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>("0.1"s, "<unit>")));
}

TEST_CASE("grammar number `;hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>(";hello_world"s, "<unit>")));
}

TEST_CASE("grammar number `\\0hello_world`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>("\0hello_world"s, "<unit>")));
}

TEST_CASE("grammar number `\\xf3\\x0c\\x00\\x00`") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
      tao::pegtl::string_input<>("\xf3\x0c\x00\x00"s, "<unit>")));
}
