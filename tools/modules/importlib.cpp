//! evaluates importlib to construct the importlib module.
//! Then prints the module construction.

#include "asdl/asdl.hpp"
#include "modules.hpp"
#include "object/object.hpp"
#include "options.hpp"
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
    virtual_machine::GlobalContext globalContext({});
    virtual_machine::ProcessContext processContext{globalContext};
    auto module = processContext.parse_file(std::cin, "<input>");
    auto main = processContext.make_module("importlib");
    virtual_machine::ThreadContext threadContext{processContext, main};
    threadContext.evaluate(module);
    const Printer printer{threadContext.body(), "importlib"};
    std::cout << printer;
  }
} // namespace chimera::library

// NOLINTNEXTLINE(bugprone-exception-escape)
auto main() -> int {
  chimera::library::execute();
  return 0;
}
