#include <catch2/catch_test_macros.hpp>

#include "fuzz/cases.hpp"

using namespace std::literals;

void LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Ellipsis<0>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Expr<0>,
                                   chimera::library::grammar::rules::Stack<
                                       chimera::library::asdl::ExprImpl>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::False<0>>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::SingleInput,
                                   chimera::library::asdl::Interactive>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Keywords>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::FileInput,
                                   chimera::library::asdl::Module>(data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Name<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::None<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::NUMBER<0>>(data,
                                                                         size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Stmt<0>,
                                   chimera::library::grammar::rules::Stack<
                                       chimera::library::asdl::StmtImpl>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::STRING<0>>(data,
                                                                         size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::True<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(chimera::library::fuzz_expression_eval(data, size));
  REQUIRE_NOTHROW(chimera::library::fuzz_interactive_eval(data, size));
  REQUIRE_NOTHROW(chimera::library::fuzz_file_eval(data, size));
}

template <typename Data>
void TestOne(Data &&data, std::size_t size) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t *>(data), size);
}

TEST_CASE(R"(fuzz `()`)") {
  auto test_case = R"(())"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\Uffffffffffffffffffff'`)") {
  auto test_case = R"('\Uffffffffffffffffffff')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\xffff'`)") {
  auto test_case = R"('\xffff')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\7777'`)") {
  auto test_case = R"('\7777')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `raise`)") {
  auto test_case = R"(raise)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0o40000000000`)") {
  auto test_case = "0o40000000000"sv;
  TestOne(test_case.data(), test_case.size());
}

// TEST_CASE(R"(fuzz `False.x=x=1`)") {
//   auto test_case = "False.x=x=1"sv;
//   TestOne(test_case.data(), test_case.size());
// }
