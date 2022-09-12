#include "grammar/rules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl.hpp>

using namespace std::literals;

namespace chimera::library {
  template <typename Data>
  auto test_parse(Data &&data) -> bool {
    Options options;
    options.chimera = "chimera";
    options.script = "unit_test.py";
    const object::Object builtins;
    virtual_machine::modules::builtins(builtins);
    const virtual_machine::VirtualMachine virtualMachine(options, builtins);
    auto processContext = virtualMachine.process_context();
    try {
      processContext.parse_file(data, "<unit_tests/virtual_machine/parse.cpp>");
    } catch (const tao::pegtl::parse_error &) {
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
