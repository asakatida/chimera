#include "object/object.hpp"                   // for Object
#include "options.hpp"                         // for Options
#include "virtual_machine/virtual_machine.hpp" // for init, ProcessContext, ...

namespace chimera {
  namespace library {
    template <typename Data>
    void test_parse(Data &&data) {
      Options options;
      options.chimera = "chimera";
      options.script = "fuzzer.py";
      object::Object builtins;
      virtual_machine::modules::init(builtins);
      virtual_machine::VirtualMachine virtualMachine(options, builtins);
      virtual_machine::ProcessContext processContext{
          virtualMachine.global_context};
      (void)processContext.parse_file(data, "<unit_test>");
    }
  } // namespace library
} // namespace chimera
