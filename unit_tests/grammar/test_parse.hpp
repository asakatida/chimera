#pragma once

#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

namespace chimera::library {
  template <typename... Data>
  void test_parse(Data &&...data) {
    Options options;
    options.chimera = "chimera";
    options.script = "unit_test.py";
    const object::Object builtins;
    virtual_machine::modules::builtins(builtins);
    const virtual_machine::VirtualMachine virtualMachine(options, builtins);
    auto processContext = virtualMachine.process_context();
    std::istringstream in(std::forward<Data>(data)...);
    auto module = processContext.parse_file(std::move(in), "<unit_test>");
    virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    threadContext.evaluate(module);
  }
} // namespace chimera::library
