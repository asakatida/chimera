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

//! global context singleton

#pragma once

#include <atomic>

#include "object/object.hpp"
#include "options.hpp"

namespace chimera::library::virtual_machine {
  struct GlobalContext {
    [[nodiscard]] auto builtins() const -> const object::Object &;
    auto interactive() -> int;
    auto execute_script() -> int;
    auto execute_script_string() -> int;
    auto execute_script_input() -> int;
    auto execute_module() -> int;
    void process_interrupts() const;
    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    const Options options;
    const object::Object builtins_;
    std::atomic_flag *sig_int;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
  };
} // namespace chimera::library::virtual_machine
