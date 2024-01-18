//! pushes either the python singleton True or False to the top of stack

#include "virtual_machine/to_bool_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  ToBoolEvaluator::ToBoolEvaluator(object::Object object)
      : object(std::move(object)) {}
  void ToBoolEvaluator::operator()(Evaluator *evaluator) const {
    if (object.get<object::False>() || object.get<object::None>()) {
      evaluator->push(PushStack{evaluator->builtins().get_attribute("False")});
    } else if (object.get<object::True>()) {
      evaluator->push(PushStack{evaluator->builtins().get_attribute("True")});
    } else {
      evaluator->push([](Evaluator *evaluatorB) {
        evaluatorB->push(ToBoolEvaluator{evaluatorB->stack_remove()});
      });
      evaluator->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluator->get_attribute(object, "__bool__");
    }
  }
} // namespace chimera::library::virtual_machine
