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

#include <csignal>

using namespace std::literals;
extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

namespace chimera {
  namespace library {
    namespace virtual_machine {
      VirtualMachine::VirtualMachine(Options options, object::Object builtins)
          : global_context{options, builtins,
                           builtins.get_attribute("type")
                               .get_attribute("__dir__")
                               .get_attribute("__class__")
                               .id(),
                           builtins.get_attribute("compile")
                               .get_attribute("__class__")
                               .id(),
                           &SIG_INT} {
        SIG_INT.test_and_set();
        std::signal(SIGINT, interupt_handler);
        builtins.set_attribute("__debug__"s,
                               options.debug ? builtins.get_attribute("True")
                                             : builtins.get_attribute("False"));
      }

      ProcessContext VirtualMachine::process_context() const {
        return ProcessContext{global_context};
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
