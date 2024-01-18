//! consumes objects of the stack to form a tuple

#pragma once

#include <cstddef>

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct TupleEvaluator {
    explicit TupleEvaluator(std::size_t size) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    std::size_t size;
  };
} // namespace chimera::library::virtual_machine
