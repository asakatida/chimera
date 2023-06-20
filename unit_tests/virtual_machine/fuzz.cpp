#include "virtual_machine/fuzz.hpp"

#include "fuzz/cases.hpp"

#include <json/contrib/map_traits.hpp>

#include <catch2/catch_test_macros.hpp>

#include <tuple>

using namespace std::literals;

namespace chimera::library {
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  void FuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
    std::ignore = fuzz_parse<grammar::Ellipsis<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::Expr<0>, grammar::rules::Stack<asdl::ExprImpl>>(
            data, size);
    std::ignore = fuzz_parse<grammar::False<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::SingleInput, asdl::Interactive>(data, size);
    std::ignore = fuzz_parse<grammar::Keywords>(data, size);
    std::ignore = fuzz_parse<grammar::FileInput, asdl::Module>(data, size);
    std::ignore = fuzz_parse<grammar::Name<0>>(data, size);
    std::ignore = fuzz_parse<grammar::None<0>>(data, size);
    std::ignore = fuzz_parse<grammar::NUMBER<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::Stmt<0>, grammar::rules::Stack<asdl::StmtImpl>>(
            data, size);
    std::ignore = fuzz_parse<grammar::STRING<0>>(data, size);
    std::ignore = fuzz_parse<grammar::True<0>>(data, size);
    std::ignore = fuzz_expression_eval(data, size);
    std::ignore = fuzz_interactive_eval(data, size);
    std::ignore = fuzz_file_eval(data, size);
  }
} // namespace chimera::library

void TestOne(const std::string &name, std::string_view data) {
  SECTION(name) {
    CHECK_NOTHROW(chimera::library::FuzzerTestOneInput(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<const std::uint8_t *>(data.data()), data.size()));
  }
}

TEST_CASE(R"(fuzz big list of nasty strings base64)") {
  const auto cases = strings_blns_base64();
  for (auto &case_ : cases.get_array()) {
    TestOne("loop base64 "s + case_.get_string(), case_.get_string());
  }
}

TEST_CASE(R"(fuzz big list of nasty strings)") {
  const auto cases = strings_blns();
  for (auto &case_ : cases.get_array()) {
    TestOne("loop blns "s + case_.get_string(), case_.get_string());
  }
}

TEST_CASE(R"(fuzz discovered cases)") {
  const auto cases = fuzz_cases();
  for (auto &case_ : cases.get_object()) {
    TestOne("loop sha "s + case_.first, case_.second.get_string());
  }
}
