//! handles individual targets of delete expressions like
//! del a, b, c
//!     ^

#include "virtual_machine/del_evaluator.hpp"

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  DelEvaluator::DelEvaluator(Evaluator *evaluator) noexcept
      : evaluator(evaluator) {}
  void DelEvaluator::evaluate(const asdl::Attribute &attribute) const {
    evaluator->push([&attribute](Evaluator *evaluatorA) {
      auto top = evaluatorA->stack_top();
      top.delete_attribute(attribute.attr.value);
      evaluatorA->stack_pop();
    });
    evaluator->evaluate_get(attribute.value);
  }
  void DelEvaluator::evaluate(const asdl::Subscript &subscript) const {
    evaluator->evaluate_get(subscript.value);
  }
  void DelEvaluator::evaluate(const asdl::Starred &starred) const {
    evaluator->evaluate_get(starred.value);
  }
  void DelEvaluator::evaluate(const asdl::Name &name) const {
    evaluator->get_attribute(evaluator->self(), name.value);
  }
} // namespace chimera::library::virtual_machine
