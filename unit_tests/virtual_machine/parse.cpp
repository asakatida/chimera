#include "grammar/grammar.hpp"
#include "grammar/rules.hpp"
#include "grammar/rules/control.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/trace.hpp>

#include <sstream>

using namespace std::literals;

namespace chimera::library {
  void test_parse(std::string_view data, std::size_t size) {
    const Options options{.chimera = "chimera",
                          .exec = options::Script{"unit_test.py"}};
    auto globalContext = virtual_machine::make_global(options);
    const auto processContext = virtual_machine::make_process(globalContext);
    std::istringstream istream{std::string{data}};
    auto module = processContext->parse_file(
        istream, "<unit_tests/virtual_machine/parse.cpp>");
    REQUIRE(module.iter().size() == size);
  }
} // namespace chimera::library

TEST_CASE("virtual machine parse ``") {
  REQUIRE_NOTHROW(chimera::library::test_parse(""sv, 0));
}

TEST_CASE("virtual machine parse `a`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("a"sv, 1));
}

TEST_CASE("virtual machine parse `hello_world`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("hello_world"sv, 1));
}

TEST_CASE("virtual machine parse `;hello_world`") {
  REQUIRE_NOTHROW(chimera::library::test_parse(";hello_world"sv, 1));
}

TEST_CASE("virtual machine parse `\\0hello_world`") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("\0hello_world"sv, -1),
                    tao::pegtl::parse_error);
}

TEST_CASE("virtual machine parse `0=help`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("0=help"sv, 1));
}

TEST_CASE("virtual machine parse `if True:\\n\\t  False`") {
  REQUIRE_THROWS_AS(chimera::library::test_parse("if True:\n\t  False"sv, -1),
                    tao::pegtl::parse_error);
}

TEST_CASE("virtual machine parse `if True:\\n\\tFalse`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("if True:\n\tFalse"sv, 1));
}

TEST_CASE("virtual machine parse `if True:\\n  False`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("if True:\n  False"sv, 1));
}

TEST_CASE("virtual machine parse `if True:\\n  False\\nelse:\\n  True`") {
  REQUIRE_NOTHROW(
      chimera::library::test_parse("if True:\n  False\nelse:\n  True"sv, 1));
}

TEST_CASE("virtual machine parse `if True:\\n  False\\nTrue\\nFalse`") {
  REQUIRE_NOTHROW(
      chimera::library::test_parse("if True:\n  False\nTrue\nFalse"sv, 3));
}

TEST_CASE("virtual machine parse `raise`") {
  REQUIRE_NOTHROW(chimera::library::test_parse("raise"sv, 1));
}
