#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>

#include <gsl/gsl>
#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  chimera::library::Options options;
  options.chimera = "chimera";
  options.script = "fuzzer.py";
  chimera::library::object::Object builtins;
  chimera::library::virtual_machine::modules::init(builtins);
  chimera::library::virtual_machine::VirtualMachine virtualMachine(options,
                                                                   builtins);
  chimera::library::virtual_machine::ProcessContext processContext{
      virtualMachine.global_context};
  chimera::library::asdl::Module module;
  bool success = true;
  std::istringstream in(
      std::string(reinterpret_cast<const char *>(data), size));
  try {
    module = processContext.parse_file(&in, "<fuzz>");
  } catch (const tao::pegtl::parse_error &) {
    success = false;
  }
  if (!success) {
    return 0;
  }
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  try {
    threadContext.evaluate(module);
  } catch (const std::exception &) {
  }
  return 0;
}
