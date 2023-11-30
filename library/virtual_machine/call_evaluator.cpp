//! handles expressions like
//! a(b, c=None)

#include "virtual_machine/call_evaluator.hpp"

#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"

#include <gsl/assert> // for Expects

#include <tuple>
#include <utility>

namespace chimera::library::virtual_machine {
  struct UnpackCallObject {
    template <typename Value>
    [[noreturn]] void evaluate(const Evaluator * /*evaluator*/,
                               const Value & /*exprImpl*/) const {
      Expects(false);
    }
    void operator()(Evaluator *evaluator) const {
      auto object = evaluator->stack_remove();
      object.visit([this, evaluator](auto &&value) {
        this->evaluate(evaluator, value);
      });
    }
  };
  CallEvaluator::CallEvaluator(object::Object object) noexcept
      : object(std::move(object)) {}
  CallEvaluator::CallEvaluator(object::Object object,
                               object::Tuple args) noexcept
      : object(std::move(object)), args(std::move(args)) {}
  void CallEvaluator::operator()(Evaluator *evaluatorA) const {
    if (object.get<object::Instance>()) {
      evaluatorA->push([](Evaluator *evaluatorB) {
        std::ignore = evaluatorB->stack_remove();
      });
      evaluatorA->get_attribute(object, "__call__");
    } else {
      evaluatorA->push(UnpackCallObject{});
    }
  }
} // namespace chimera::library::virtual_machine
