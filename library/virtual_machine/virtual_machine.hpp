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

//! controller to set up GlobalContext.

#pragma once

#include <atomic> // for atomic_flag

#include "object/object.hpp"                   // for Object
#include "options.hpp"                         // for Options
#include "virtual_machine/builtins.hpp"        // for init
#include "virtual_machine/global_context.hpp"  // for GlobalContext
#include "virtual_machine/process_context.hpp" // for ProcessContext
#include "virtual_machine/thread_context.hpp"  // for ThreadContext

static std::atomic_flag SIG_INT;

extern "C" void interupt_handler(int signal);

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct VirtualMachine {
        VirtualMachine(Options options, object::Object builtins);

        ProcessContext process_context() const;

        GlobalContext global_context;
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
