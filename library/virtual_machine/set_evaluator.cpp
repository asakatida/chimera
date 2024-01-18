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
