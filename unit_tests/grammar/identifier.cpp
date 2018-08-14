#include "grammar/identifier.hpp"

#include <catch2/catch.hpp>

using namespace std::literals;

TEST_CASE("grammar identifier ()") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>(""s, "<unit>")));
}

TEST_CASE("grammar identifier (a)") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>("a"s, "<unit>")));
}

TEST_CASE("grammar identifier (hello_world)") {
  REQUIRE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>("hello_world"s, "<unit>")));
}

TEST_CASE("grammar identifier (;hello_world)") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>(";hello_world"s, "<unit>")));
}

TEST_CASE("grammar identifier (\\0hello_world)") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>("\0hello_world"s, "<unit>")));
}

TEST_CASE("grammar identifier (\\xf3\\x0c\\x00\\x00)") {
  REQUIRE_FALSE(tao::pegtl::parse<chimera::library::grammar::Name<0>>(
      tao::pegtl::string_input<>("\xf3\x0c\x00\x00"s, "<unit>")));
}
