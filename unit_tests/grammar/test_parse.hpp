#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

namespace chimera {
  namespace library {
    template <typename Data>
    auto test_parse(Data &&data) {
      Options options;
      options.chimera = "chimera";
      options.script = "unit_test.py";
      object::Object builtins;
      virtual_machine::modules::init(builtins);
      virtual_machine::VirtualMachine virtualMachine(options, builtins);
      auto processContext = virtualMachine.process_context();
      std::istringstream in(data);
      auto module = processContext.parse_file(in, "<unit_test>");
      virtual_machine::ThreadContext threadContext{
          processContext, processContext.make_module("__main__")};
      threadContext.evaluate(module);
    }
  } // namespace library
} // namespace chimera
