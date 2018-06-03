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

namespace chimera {
  namespace library {
    namespace virtual_machine {
      void SetEvaluator::evaluate(const asdl::Attribute &attribute) {
        evaluator->push([&attribute](Evaluator *evaluatorA) {
          auto value = std::move(evaluatorA->stack.top());
          evaluatorA->stack.pop();
          evaluatorA->stack.top().set_attribute(attribute.attr.value,
                                                std::move(value));
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(attribute.value);
      }
      void SetEvaluator::evaluate(const asdl::Subscript &subscript) {
        evaluator->evaluate_get(subscript.value);
      }
      void SetEvaluator::evaluate(const asdl::Name &name) {
        evaluator->push([&name](Evaluator *evaluatorA) {
          evaluatorA->self().set_attribute(name.value, evaluatorA->stack.top());
        });
      }
      void SetEvaluator::evaluate(const asdl::List & /*list*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void SetEvaluator::evaluate(const asdl::Tuple & /*tuple*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
