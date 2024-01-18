//! pushes either the python singleton True or False to the top of stack

#pragma once

#include "object/object.hpp"

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct ToBoolEvaluator {
    explicit ToBoolEvaluator(object::Object object);
    void operator()(Evaluator *evaluator) const;

  private:
    object::Object object;
  };
} // namespace chimera::library::virtual_machine
