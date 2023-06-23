// Copyright (c) 2018 Asa Katida <github@holomaplefeline.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
