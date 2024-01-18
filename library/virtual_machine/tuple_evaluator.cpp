//! consumes objects of the stack to form a tuple

#include "virtual_machine/tuple_evaluator.hpp"

#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  TupleEvaluator::TupleEvaluator(std::size_t size) noexcept : size(size) {}
  void TupleEvaluator::operator()(Evaluator *evaluator) const {
    object::Tuple tuple(evaluator->stack_size() - size);
    for (std::size_t idx = 0; evaluator->stack_size() > size; ++idx) {
      tuple[tuple.size() - 1 - idx] = evaluator->stack_top();
      evaluator->stack_pop();
    }
    evaluator->push(PushStack{object::Object(
        tuple, {{"__class__", evaluator->builtins().get_attribute("tuple")}})});
  }
} // namespace chimera::library::virtual_machine
