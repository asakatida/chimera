#include "builtins/builtins.hpp"
#include "grammar/rules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl.hpp>

using namespace std::literals;

namespace chimera::library {
  template <typename Data>
  auto test_parse(Data &&data) -> bool {
    Options options;
    options.chimera = "chimera";
    options.script = "unit_test.py";
    const virtual_machine::GlobalContext globalContext(options);
    virtual_machine::ProcessContext processContext{globalContext};
    try {
      processContext.parse_file(data, "<unit_tests/virtual_machine/parse.cpp>");
    } catch (const tao::pegtl::parse_error &) {
      return false;
    } catch (const grammar::SyntaxError &) {
      return false;
    }
    return true;
  }
} // namespace chimera::library

TEST_CASE("virtual machine parse ()") {
  REQUIRE(chimera::library::test_parse(""s));
}

TEST_CASE("virtual machine parse (a)") {
  REQUIRE(chimera::library::test_parse("a"s));
}

TEST_CASE("virtual machine parse (hello_world)") {
  REQUIRE(chimera::library::test_parse("hello_world"s));
}

TEST_CASE("virtual machine parse (;hello_world)") {
  REQUIRE(chimera::library::test_parse(";hello_world"s));
}

TEST_CASE("virtual machine parse (\\0hello_world)") {
  REQUIRE_FALSE(chimera::library::test_parse("\0hello_world"s));
}

TEST_CASE("virtual machine parse (0=help)") {
  REQUIRE(chimera::library::test_parse("0=help"s));
}
