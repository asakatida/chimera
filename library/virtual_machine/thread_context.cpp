//! thread context attached to an evaluator thread

#include "virtual_machine/thread_context.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  ThreadContext::ThreadContext(ProcessContext &process_context,
                               object::Object main)
      : process_context(gsl::make_not_null(&process_context)),
        main(std::move(main)) {}
  [[nodiscard]] auto ThreadContext::body() const -> object::Object {
    return main;
  }
  [[nodiscard]] auto ThreadContext::builtins() const -> const object::Object & {
    return process_context->builtins();
  }
  void ThreadContext::evaluate(const asdl::Module &module) {
    return Evaluator{*this}.evaluate(module);
  }
  void ThreadContext::evaluate(const asdl::Interactive &interactive) {
    return Evaluator{*this}.evaluate(interactive);
  }
  void ThreadContext::evaluate(const asdl::Expression &expression) {
    return Evaluator{*this}.evaluate(expression);
  }
  void ThreadContext::process_interrupts() const {
    process_context->process_interrupts();
  }
  [[nodiscard]] auto ThreadContext::return_value() const -> object::Object {
    return ret.value_or(builtins().get_attribute("None"));
  }
  void ThreadContext::return_value(object::Object &&value) {
    ret = std::move(value);
  }
} // namespace chimera::library::virtual_machine
