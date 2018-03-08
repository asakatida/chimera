#include <cstddef> // for size_t
#include <cstdint>
#include <exception> // for exception
#include <sstream>   // for istringstream
#include <string>

#include <tao/pegtl.hpp> // for parse_error

#include "asdl/asdl.hpp"                       // for Module
#include "object/object.hpp"                   // for Object
#include "options.hpp"                         // for Options
#include "virtual_machine/virtual_machine.hpp" // for init, ProcessContext, ...

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

namespace chimera {
  namespace library {
    static void test(const uint8_t *data, const size_t size) {
      try {
        Options options{};
        options.chimera = "chimera";
        options.script = "fuzzer.py";
        object::Object builtins;
        virtual_machine::modules::init(builtins);
        virtual_machine::VirtualMachine virtualMachine(options, builtins);
        auto processContext = virtualMachine.process_context();
        asdl::Module module;
        std::istringstream in(std::string(reinterpret_cast<const char *>(data), size));
        try {
          module = processContext.parse_file(in, "<fuzz>");
        } catch (const tao::pegtl::parse_error &) {
          return;
        }
        virtual_machine::ThreadContext threadContext{
            processContext, processContext.make_module("__main__")};
        try {
          threadContext.evaluate(module);
        } catch (const std::exception &) {
        }
      } catch (...) {
        Ensures(false);
      }
    }
  } // namespace library
} // namespace chimera

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  chimera::library::test(data, size);
  return 0;
}
