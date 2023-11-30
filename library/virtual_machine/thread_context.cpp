//! thread context attached to an evaluator thread

#include "virtual_machine/thread_context.hpp"

#include "object/object.hpp"
#include "virtual_machine/process_context.hpp"

#include <memory>
#include <utility>

namespace chimera::library::virtual_machine {
  ThreadContextImpl::ThreadContextImpl(ProcessContext &process_context,
                                       object::Object main)
      : process_context(process_context), main(std::move(main)) {}
  [[nodiscard]] auto ThreadContextImpl::body() const -> object::Object {
    return main;
  }
  [[nodiscard]] auto
  ThreadContextImpl::builtins() const -> const object::Object & {
    return process_context->builtins();
  }
  void ThreadContextImpl::process_interrupts() const {
    process_context->process_interrupts();
  }
  [[nodiscard]] auto ThreadContextImpl::return_value() const -> object::Object {
    return ret.value_or(builtins().get_attribute("None"));
  }
  void ThreadContextImpl::return_value(object::Object &&value) {
    ret = std::move(value);
  }
  auto make_thread(ProcessContext &process_context,
                   object::Object main) -> ThreadContext {
    return std::make_shared<ThreadContextImpl>(process_context,
                                               std::move(main));
  }
} // namespace chimera::library::virtual_machine
