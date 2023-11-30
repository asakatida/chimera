//! thread context attached to an evaluator thread

#pragma once

#include "asdl/asdl.hpp"
#include "object/object.hpp"
#include "virtual_machine/process_context.hpp"

#include <optional>

namespace chimera::library::virtual_machine {
  struct ThreadContextImpl {
    ThreadContextImpl(ProcessContext &process_context, object::Object main);
    [[nodiscard]] auto body() const -> object::Object;
    [[nodiscard]] auto builtins() const -> const object::Object &;
    template <typename... Args>
    [[nodiscard]] auto import_object(Args &&...args) -> const object::Object & {
      return process_context->import_object(std::forward<Args>(args)...);
    }
    void process_interrupts() const;
    [[nodiscard]] auto return_value() const -> object::Object;
    void return_value(object::Object &&value);

  private:
    ProcessContext process_context;
    object::Object main;
    std::optional<object::Object> ret;
  };
  using ThreadContext = std::shared_ptr<ThreadContextImpl>;
  auto make_thread(ProcessContext &process_context, object::Object main)
      -> ThreadContext;
} // namespace chimera::library::virtual_machine
