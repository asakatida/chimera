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
//! not a

#include "virtual_machine/unary_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  void UnaryBitNotEvaluator::operator()(Evaluator *evaluatorA) const {
    evaluatorA->push([](Evaluator *evaluator) {
      evaluator->push(CallEvaluator{evaluator->stack_remove()});
    });
    evaluatorA->get_attribute(evaluatorA->stack_top(), "__invert__");
    evaluatorA->stack_pop();
  }
  void UnaryNotEvaluator::operator()(Evaluator *evaluatorA) const {
    evaluatorA->push([](Evaluator *evaluatorB) {
      if (evaluatorB->stack_top().get_bool()) {
        evaluatorB->stack_top_update(
            evaluatorB->builtins().get_attribute("False"));
      } else {
        evaluatorB->stack_top_update(
            evaluatorB->builtins().get_attribute("True"));
      }
    });
    evaluatorA->push(ToBoolEvaluator{evaluatorA->stack_top()});
    evaluatorA->stack_pop();
  }
  void UnaryAddEvaluator::operator()(Evaluator *evaluatorA) const {
    evaluatorA->push([](Evaluator *evaluatorB) {
      evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
    });
    evaluatorA->get_attribute(evaluatorA->stack_top(), "__pos__");
    evaluatorA->stack_pop();
  }
  void UnarySubEvaluator::operator()(Evaluator *evaluatorA) const {
    evaluatorA->push([](Evaluator *evaluatorB) {
      evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
    });
    evaluatorA->get_attribute(evaluatorA->stack_top(), "__neg__");
    evaluatorA->stack_pop();
  }
} // namespace chimera::library::virtual_machine
