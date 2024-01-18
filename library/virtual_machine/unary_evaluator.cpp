//! handles expressions like
//! not a

#include "virtual_machine/unary_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  void UnaryBitNotEvaluator::operator()(Evaluator *evaluator) const {
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(CallEvaluator{evaluatorA->stack_remove()});
    });
    evaluator->get_attribute(evaluator->stack_top(), "__invert__");
    evaluator->stack_pop();
  }
  void UnaryNotEvaluator::operator()(Evaluator *evaluator) const {
    evaluator->push([](Evaluator *evaluatorA) {
      if (evaluatorA->stack_top().get_bool()) {
        evaluatorA->stack_top_update(
            evaluatorA->builtins().get_attribute("False"));
      } else {
        evaluatorA->stack_top_update(
            evaluatorA->builtins().get_attribute("True"));
      }
    });
    evaluator->push(ToBoolEvaluator{evaluator->stack_top()});
    evaluator->stack_pop();
  }
  void UnaryAddEvaluator::operator()(Evaluator *evaluator) const {
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(CallEvaluator{evaluatorA->stack_remove()});
    });
    evaluator->get_attribute(evaluator->stack_top(), "__pos__");
    evaluator->stack_pop();
  }
  void UnarySubEvaluator::operator()(Evaluator *evaluator) const {
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(CallEvaluator{evaluatorA->stack_remove()});
    });
    evaluator->get_attribute(evaluator->stack_top(), "__neg__");
    evaluator->stack_pop();
  }
} // namespace chimera::library::virtual_machine
