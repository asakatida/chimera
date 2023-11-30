//! handles expressions like
//! a and b and c

#include "virtual_machine/bool_evaluator.hpp"

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/to_bool_evaluator.hpp"

#include <vector>

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
