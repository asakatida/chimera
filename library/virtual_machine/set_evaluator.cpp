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

//! handles individual targets of assignment expressions like
//! a = b = c = ...
//!     ^

#include "virtual_machine/set_evaluator.hpp"

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  SetEvaluator::SetEvaluator(Evaluator *evaluator) noexcept
      : evaluator(evaluator) {}
  void SetEvaluator::evaluate(const asdl::Attribute &attribute) const {
    evaluator->push([&attribute](Evaluator *evaluatorA) {
      auto object = evaluatorA->stack_remove();
      auto top = evaluatorA->stack_top();
      top.set_attribute(attribute.attr.value, std::move(object));
      evaluatorA->stack_pop();
    });
    evaluator->evaluate_get(attribute.value);
  }
  void SetEvaluator::evaluate(const asdl::Subscript &subscript) const {
    evaluator->evaluate_get(subscript.value);
  }
  void SetEvaluator::evaluate(const asdl::Name &name) const {
    evaluator->push([&name](Evaluator *evaluatorA) {
      evaluatorA->self().set_attribute(name.value, evaluatorA->stack_top());
    });
  }
  void SetEvaluator::evaluate(const asdl::List & /*list*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void SetEvaluator::evaluate(const asdl::Tuple & /*tuple*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
} // namespace chimera::library::virtual_machine
