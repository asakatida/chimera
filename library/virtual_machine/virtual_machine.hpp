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

#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"

namespace chimera::library::virtual_machine {
  struct VirtualMachine {
    explicit VirtualMachine(const Options &options);
    [[nodiscard]] auto execute_module() -> int;
    [[nodiscard]] auto execute_script_input() -> int;
    [[nodiscard]] auto execute_script_string() -> int;
    [[nodiscard]] auto execute_script() -> int;
    [[nodiscard]] auto interactive() -> int;
    [[nodiscard]] auto process_context() const -> ProcessContext;

  private:
    GlobalContext global_context;
  };
} // namespace chimera::library::virtual_machine
