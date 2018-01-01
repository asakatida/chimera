#include <atomic> // for atomic_flag
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
  std::atomic_flag SIG_INT{};
  SIG_INT.test_and_set();
  chimera::library::virtual_machine::GlobalContext globalContext{
      options, builtins,
      builtins.get_attribute("type")
          .get_attribute("__dir__")
          .get_attribute("__class__")
          .id(),
      builtins.get_attribute("compile").get_attribute("__class__").id(),
      &SIG_INT};
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
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
