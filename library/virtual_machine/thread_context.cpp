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

#include "virtual_machine/thread_context.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  ThreadContext::ThreadContext(ProcessContext &process_context,
                               object::Object main)
      : process_context(gsl::make_not_null(&process_context)),
        main(std::move(main)) {}
  [[nodiscard]] auto ThreadContext::body() const -> object::Object {
    return main;
  }
  [[nodiscard]] auto ThreadContext::builtins() const -> const object::Object & {
    return process_context->builtins();
  }
  void ThreadContext::evaluate(const asdl::Module &module) {
    return Evaluator{*this}.evaluate(module);
  }
  void ThreadContext::evaluate(const asdl::Interactive &interactive) {
    return Evaluator{*this}.evaluate(interactive);
  }
  void ThreadContext::evaluate(const asdl::Expression &expression) {
    return Evaluator{*this}.evaluate(expression);
  }
  void ThreadContext::process_interrupts() const {
    process_context->process_interrupts();
  }
  [[nodiscard]] auto ThreadContext::return_value() const -> object::Object {
    return ret.value_or(builtins().get_attribute("None"));
  }
  void ThreadContext::return_value(object::Object &&value) {
    ret = std::move(value);
  }
} // namespace chimera::library::virtual_machine
