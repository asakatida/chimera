#include "virtual_machine/virtual_machine.hpp"

#include <catch2/catch_test_macros.hpp>

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
    REQUIRE_NOTHROW(threadContext.evaluate(module));
  }
  REQUIRE(true);
}

TEST_CASE("grammar VirtualMachine (a@b=c)") {
  chimera::library::Options options;
  options.chimera = "chimera";
  options.script = "test.py";
  chimera::library::object::Object builtins;
  chimera::library::virtual_machine::modules::init(builtins);
  chimera::library::virtual_machine::VirtualMachine virtualMachine(options,
                                                                   builtins);
  auto processContext = virtualMachine.process_context();
  auto module = processContext.parse_file("a@b=c"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  try {
    threadContext.evaluate(module);
  } catch (const std::exception &) {
  } catch (...) {
  }
}
