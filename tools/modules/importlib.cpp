//! evaluates importlib to construct the importlib module.
//! Then prints the module construction.

#include "modules.hpp"
#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <exception>
#include <iostream>

namespace chimera::library {
  // NOLINTNEXTLINE(misc-use-anonymous-namespace)
  static void execute() {
    auto globalContext = virtual_machine::make_global({});
    auto processContext = virtual_machine::make_process(globalContext);
    auto module = processContext->parse_file(std::cin, "<input>");
    auto main = processContext->make_module("importlib");
    auto threadContext = virtual_machine::make_thread(processContext, main);
    virtual_machine::Evaluator(threadContext).evaluate(module);
    const Printer printer{threadContext->body(), "importlib"};
    std::cout << printer;
  }
} // namespace chimera::library

auto main() -> int {
  try {
    try {
      chimera::library::execute();
      return 0;
    } catch (const std::exception &error) {
      std::cerr << error.what() << '\n';
      return 1;
    }
  } catch (...) {
    return 254;
  }
  return 1;
}
