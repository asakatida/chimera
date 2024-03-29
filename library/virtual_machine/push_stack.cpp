//! pushes top of stack when evaluated

#include "virtual_machine/push_stack.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  PushStack::PushStack(object::Object object) : object(std::move(object)) {}
  void PushStack::operator()(Evaluator *evaluator) const {
    evaluator->stack_push(object);
  }
} // namespace chimera::library::virtual_machine
