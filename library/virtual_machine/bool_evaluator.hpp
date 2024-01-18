//! handles expressions like
//! a and b and c

#pragma once

#include "asdl/asdl.hpp"

#include <vector>

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct BoolAndEvaluator {
    explicit BoolAndEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BoolAndEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BoolOrEvaluator {
    explicit BoolOrEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BoolOrEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
} // namespace chimera::library::virtual_machine
