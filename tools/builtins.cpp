// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
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

//! evaluates stdlib/_builtins.py to construct the builtin module.
//! Then prints the module construction.

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "asdl/asdl.hpp"
#include "modules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/virtual_machine.hpp"

namespace chimera::library {
  static void main() {
    object::Object builtins;
    virtual_machine::modules::init(builtins);
    virtual_machine::VirtualMachine virtualMachine({}, builtins);
    auto processContext = virtualMachine.process_context();
    auto module = processContext.parse_file(std::cin, "<input>");
    virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("builtins")};
    threadContext.evaluate(module);
    Printer printer{threadContext.main, "builtins",
                    threadContext.main.get_attribute("__builtins__")};
    std::cout << printer;
  }
} // namespace chimera::library

auto main() -> int {
  chimera::library::main();
  return 0;
}
