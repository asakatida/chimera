//! pushes top of stack when evaluated

#pragma once

#include "object/object.hpp"

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct PushStack {
    explicit PushStack(object::Object object);
    void operator()(Evaluator *evaluator) const;

  private:
    object::Object object;
  };
} // namespace chimera::library::virtual_machine
