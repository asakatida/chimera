#include "grammar/grammar.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl/contrib/analyze.hpp>

TEST_CASE("grammar analyze `SingleInput`") {
  REQUIRE(tao::pegtl::analyze<chimera::library::grammar::SingleInput>() == 0);
}

TEST_CASE("grammar analyze `FileInput`") {
  REQUIRE(tao::pegtl::analyze<chimera::library::grammar::FileInput>() == 0);
}

TEST_CASE("grammar analyze `EvalInput`") {
  REQUIRE(tao::pegtl::analyze<chimera::library::grammar::EvalInput>() == 0);
}
