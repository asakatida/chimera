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

//! constructs and processes an evaluator

#include "virtual_machine/virtual_machine.hpp"

#include <atomic>
#include <csignal>

using namespace std::literals;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::atomic_flag SIG_INT{};

extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

namespace chimera::library::virtual_machine {
  VirtualMachine::VirtualMachine(const Options &options,
                                 object::Object builtins)
      : global_context{
            options, builtins,
            builtins.get_attribute("type")
                .get_attribute("__dir__")
                .get_attribute("__class__")
                .id(),
            builtins.get_attribute("compile").get_attribute("__class__").id(),
            &SIG_INT} {
    SIG_INT.test_and_set();
    std::ignore = std::signal(SIGINT, interupt_handler);
    builtins.set_attribute("__debug__"s, options.debug
                                             ? builtins.get_attribute("True")
                                             : builtins.get_attribute("False"));
  }
  auto VirtualMachine::execute_module() -> int {
    return global_context.execute_module();
  }
  auto VirtualMachine::execute_script_input() -> int {
    return global_context.execute_script_input();
  }
  auto VirtualMachine::execute_script_string() -> int {
    return global_context.execute_script_string();
  }
  auto VirtualMachine::execute_script() -> int {
    return global_context.execute_script();
  }
  auto VirtualMachine::interactive() -> int {
    return global_context.interactive();
  }
  auto VirtualMachine::process_context() const -> ProcessContext {
    return ProcessContext{global_context};
  }
} // namespace chimera::library::virtual_machine
