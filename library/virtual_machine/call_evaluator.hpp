//! handles expressions like
//! a(b, c=None)

#pragma once

#include "object/object.hpp"

#include <optional>

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct CallEvaluator {
    explicit CallEvaluator(object::Object object) noexcept;
    CallEvaluator(object::Object object, object::Tuple args) noexcept;
    void operator()(Evaluator *evaluatorA) const;

  private:
    object::Object object;
    object::Tuple args;
    std::optional<object::Object> kwargs;
  };
} // namespace chimera::library::virtual_machine
