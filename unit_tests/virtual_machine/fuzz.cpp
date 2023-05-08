#include "fuzz/cases.hpp"

#include <catch2/catch_test_macros.hpp>
#include <json.hpp>

using namespace std::literals;

namespace chimera::library {
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  void FuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
    (void)fuzz_parse<grammar::Ellipsis<0>>(data, size);
    (void)fuzz_parse<grammar::Expr<0>, grammar::rules::Stack<asdl::ExprImpl>>(
        data, size);
    (void)fuzz_parse<grammar::False<0>>(data, size);
    (void)fuzz_parse<grammar::SingleInput, asdl::Interactive>(data, size);
    (void)fuzz_parse<grammar::Keywords>(data, size);
    (void)fuzz_parse<grammar::FileInput, asdl::Module>(data, size);
    (void)fuzz_parse<grammar::Name<0>>(data, size);
    (void)fuzz_parse<grammar::None<0>>(data, size);
    (void)fuzz_parse<grammar::NUMBER<0>>(data, size);
    (void)fuzz_parse<grammar::Stmt<0>, grammar::rules::Stack<asdl::StmtImpl>>(
        data, size);
    (void)fuzz_parse<grammar::STRING<0>>(data, size);
    (void)fuzz_parse<grammar::True<0>>(data, size);
    (void)fuzz_expression_eval(data, size);
    (void)fuzz_interactive_eval(data, size);
    (void)fuzz_file_eval(data, size);
  }
} // namespace chimera::library

void TestOne(std::string_view &&data) {
  REQUIRE_NOTHROW(chimera::library::FuzzerTestOneInput(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const std::uint8_t *>(data.data()), data.size()));
}

TEST_CASE(R"(fuzz big list of nasty strings base64)") {
  auto cases = tao::json::from_file(
      "external/big-list-of-naughty-strings/blns.base64.json");
  for (auto &case_ : cases.get_array()) {
    SECTION("loop base64 "s + case_.get_string()) {
      TestOne(case_.get_string());
    }
  }
}

TEST_CASE(R"(fuzz big list of nasty strings)") {
  auto cases =
      tao::json::from_file("external/big-list-of-naughty-strings/blns.json");
  for (auto &case_ : cases.get_array()) {
    SECTION("loop blns "s + case_.get_string()) { TestOne(case_.get_string()); }
  }
}

TEST_CASE(R"(fuzz discovered cases)") {
  auto cases = tao::json::from_file("unit_tests/fuzz/cases.json");
  for (auto &case_ : cases.get_object()) {
    SECTION("loop sha "s + case_.first) { TestOne(case_.second.get_string()); }
  }
}
