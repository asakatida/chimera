//! evaluates importlib to construct the importlib module.
//! Then prints the module construction.

#include "asdl/asdl.hpp"
#include "modules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <gsl/gsl>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

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

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
  chimera::library::execute();
  return 0;
}
