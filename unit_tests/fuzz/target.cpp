#include <atomic> // for atomic_flag
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>

#include <gsl/gsl>
#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/grammar.hpp"
#include "options.hpp"
#include "parse.hpp"
#include "virtual_machine/virtual_machine.hpp"

static std::atomic_flag SIG_INT;

extern "C" void interupt_handler(int signal);

extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  chimera::Options options;
  options.chimera = "chimera";
  options.script = "fuzzer.py";
  chimera::library::object::Object builtins;
  chimera::library::virtual_machine::modules::init(builtins);
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
      std::string{reinterpret_cast<const char *>(data), size});
  try {
    Ensures(tao::pegtl::parse<chimera::library::grammar::FileInput>(
        chimera::library::grammar::Input<tao::pegtl::istream_input<>>(in, size,
                                                                      "<fuzz>"),
        processContext, module));
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
