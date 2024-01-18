//! constructs a slice object from a slice expression

#pragma once

#include "asdl/asdl.hpp"

#include <vector>

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct SliceEvaluator {
    explicit SliceEvaluator(Evaluator *evaluator) noexcept;
    void operator()(const asdl::ExtSlice &extSlice) const;
    void operator()(const asdl::Index &index) const;
    void operator()(const asdl::Slice &slice) const;

  private:
    Evaluator *evaluator;
  };
} // namespace chimera::library::virtual_machine
