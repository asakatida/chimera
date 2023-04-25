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

//! thread context attached to an evaluator thread

#pragma once

#include <optional>

#include "asdl/asdl.hpp"
#include "object/object.hpp"
#include "virtual_machine/process_context.hpp"

namespace chimera::library::virtual_machine {
  struct ThreadContext {
    ThreadContext(ProcessContext &process_context, object::Object main);
    [[nodiscard]] auto body() const -> object::Object;
    [[nodiscard]] auto builtins() const -> const object::Object &;
    void evaluate(const asdl::Module &module);
    void evaluate(const asdl::Interactive &interactive);
    void evaluate(const asdl::Expression &expression);
    template <typename... Args>
    [[nodiscard]] auto import_object(Args &&...args) -> const object::Object & {
      return process_context->import_object(std::forward<Args>(args)...);
    }
    void process_interrupts() const;
    [[nodiscard]] auto return_value() const -> object::Object;
    void return_value(object::Object &&value);

  private:
    gsl::not_null<ProcessContext *> process_context;
    object::Object main;
    std::optional<object::Object> ret;
  };
} // namespace chimera::library::virtual_machine
