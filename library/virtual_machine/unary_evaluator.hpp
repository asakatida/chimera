//! handles expressions like
//! not a

#pragma once

#include "asdl/asdl.hpp"

#include <vector>

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct UnaryBitNotEvaluator {
    void operator()(Evaluator *evaluator) const;
  };
  struct UnaryNotEvaluator {
    void operator()(Evaluator *evaluator) const;
  };
  struct UnaryAddEvaluator {
    void operator()(Evaluator *evaluator) const;
  };
  struct UnarySubEvaluator {
    void operator()(Evaluator *evaluator) const;
  };
} // namespace chimera::library::virtual_machine
