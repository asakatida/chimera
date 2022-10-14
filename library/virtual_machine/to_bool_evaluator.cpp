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

//! pushes either the python singleton True or False to the top of stack

#include "virtual_machine/to_bool_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  ToBoolEvaluator::ToBoolEvaluator(object::Object object)
      : object(std::move(object)) {}
  void ToBoolEvaluator::operator()(Evaluator *evaluator) const {
    if (std::holds_alternative<object::False>(object.value()) ||
        std::holds_alternative<object::None>(object.value())) {
      evaluator->push(PushStack{evaluator->builtins().get_attribute("False")});
    } else if (std::holds_alternative<object::True>(object.value())) {
      evaluator->push(PushStack{evaluator->builtins().get_attribute("True")});
    } else {
      evaluator->push([](Evaluator *evaluatorB) {
        evaluatorB->push(ToBoolEvaluator{evaluatorB->stack_remove()});
      });
      evaluator->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove(), {}, {}});
      });
      evaluator->get_attribute(object, "__bool__");
    }
  }
} // namespace chimera::library::virtual_machine
