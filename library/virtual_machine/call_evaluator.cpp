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

//! handles expressions like
//! a(b, c=None)

#include "virtual_machine/call_evaluator.hpp"

#include <gsl/gsl>

#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  struct UnpackCallObject {
    template <typename Value>
    [[noreturn]] void evaluate(const Evaluator * /*evaluator*/,
                               const Value & /*exprImpl*/) const {
      Expects(false);
    }
    void operator()(Evaluator *evaluator) const {
      auto object = evaluator->stack_remove();
      std::visit(
          [this, evaluator](auto &&value) { this->evaluate(evaluator, value); },
          object.value());
    }
  };
  CallEvaluator::CallEvaluator(object::Object object) noexcept
      : object(std::move(object)) {}
  CallEvaluator::CallEvaluator(object::Object object,
                               object::Tuple args) noexcept
      : object(std::move(object)), args(std::move(args)) {}
  void CallEvaluator::operator()(Evaluator *evaluatorA) const {
    if (std::holds_alternative<object::Instance>(object.value())) {
      evaluatorA->push([](Evaluator *evaluatorB) {
        std::ignore = evaluatorB->stack_remove();
      });
      evaluatorA->get_attribute(object, "__call__");
    } else {
      evaluatorA->push(UnpackCallObject{});
    }
  }
} // namespace chimera::library::virtual_machine
