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
//! a and b and c

#include "virtual_machine/bool_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  BoolAndEvaluator::BoolAndEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : begin(exprs.begin()), end(exprs.end()) {}
  BoolAndEvaluator::BoolAndEvaluator(
      const BoolAndEvaluator::Iterator &begin,
      const BoolAndEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BoolAndEvaluator::operator()(Evaluator *evaluator) const {
    if (begin != end) {
      auto object = evaluator->stack_remove();
      if (object.get_bool()) {
        evaluator->stack_pop();
        const auto &expr = *begin;
        evaluator->push(BoolAndEvaluator{begin + 1, end});
        evaluator->push([](Evaluator *evaluatorB) {
          evaluatorB->push(ToBoolEvaluator{evaluatorB->stack_top()});
        });
        evaluator->evaluate_get(expr);
      }
    }
  }
  BoolOrEvaluator::BoolOrEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : begin(exprs.begin()), end(exprs.end()) {}
  BoolOrEvaluator::BoolOrEvaluator(
      const BoolOrEvaluator::Iterator &begin,
      const BoolOrEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BoolOrEvaluator::operator()(Evaluator *evaluator) const {
    if (begin != end) {
      auto object = evaluator->stack_remove();
      if (!object.get_bool()) {
        evaluator->stack_pop();
        const auto &expr = *begin;
        evaluator->push(BoolOrEvaluator{begin + 1, end});
        evaluator->push([](Evaluator *evaluatorB) {
          evaluatorB->push(ToBoolEvaluator{evaluatorB->stack_top()});
        });
        evaluator->evaluate_get(expr);
      }
    }
  }
} // namespace chimera::library::virtual_machine
