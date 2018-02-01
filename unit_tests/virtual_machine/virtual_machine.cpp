#include "virtual_machine/virtual_machine.hpp"

#include <catch.hpp>

using namespace std::literals;

TEST_CASE("grammar VirtualMachine ()") {
  SECTION("destructor") {
    chimera::library::Options options;
    options.chimera = "chimera";
    options.script = "test.py";
    chimera::library::object::Object builtins;
    chimera::library::virtual_machine::modules::init(builtins);
    chimera::library::virtual_machine::VirtualMachine virtualMachine(options,
                                                                     builtins);
    auto processContext = virtualMachine.process_context();
    auto module = processContext.parse_file(""sv, "<test>");
    chimera::library::virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    threadContext.evaluate(module);
  }
  REQUIRE(true);
}
