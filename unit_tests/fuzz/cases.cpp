#include "fuzz/cases.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <sstream>
#include <string>

#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/number.hpp"
#include "grammar/rules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

namespace chimera::library {
  auto fuzz_file_eval(const std::uint8_t *data, std::size_t size) -> int {
    Options options{};
    options.chimera = "chimera";
    options.script = "fuzzer.py";
    const object::Object builtins;
    virtual_machine::modules::init(builtins);
    const virtual_machine::VirtualMachine virtualMachine(options, builtins);
    auto processContext = virtualMachine.process_context();
    asdl::Module module;
    std::istringstream in(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        std::string(reinterpret_cast<const char *>(data), size));
    try {
      module = processContext.parse_file(in, "<fuzz>");
    } catch (const tao::pegtl::parse_error &) {
      return 0;
    }
    virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    try {
      threadContext.evaluate(module);
    } catch (const object::BaseException &) {
    }
    return 0;
  }

  auto fuzz_parse_number(const std::uint8_t *data, std::size_t size) -> int {
    try {
      tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
          tao::pegtl::memory_input<>(reinterpret_cast<const char *>(data), size,
                                     "<fuzz>"));
    } catch (const tao::pegtl::parse_error &) {
    }
    return 0;
  }
} // namespace chimera::library
