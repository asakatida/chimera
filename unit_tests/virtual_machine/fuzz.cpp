#include <catch2/catch_test_macros.hpp>

#include "fuzz/cases.hpp"

using namespace std::literals;

void LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::Ellipsis<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::Expr<0>,
                                       chimera::library::grammar::rules::Stack<
                                           chimera::library::asdl::ExprImpl>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::False<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::SingleInput,
                                       chimera::library::asdl::Interactive>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::Keywords>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::FileInput,
                                       chimera::library::asdl::Module>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::Name<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::None<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::NUMBER<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::Stmt<0>,
                                       chimera::library::grammar::rules::Stack<
                                           chimera::library::asdl::StmtImpl>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::STRING<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_parse<chimera::library::grammar::True<0>>(
              data, size) == 0);
  REQUIRE(chimera::library::fuzz_expression_eval(data, size) == 0);
  REQUIRE(chimera::library::fuzz_interactive_eval(data, size) == 0);
  REQUIRE(chimera::library::fuzz_file_eval(data, size) == 0);
}

template <typename Data>
void TestOne(Data &&data, std::size_t size) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t *>(data), size);
}
TEST_CASE(R"(fuzz `()`)") {
  auto test_case = R"(())"sv;
  REQUIRE_NOTHROW(TestOne(test_case.data(), test_case.size()));
}
