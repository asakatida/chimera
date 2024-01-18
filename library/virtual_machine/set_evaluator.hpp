//! handles individual targets of assignment expressions like
//! a = b = c = ...
//!     ^

#pragma once

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  struct SetEvaluator {
    explicit SetEvaluator(Evaluator *evaluator) noexcept;
    void evaluate(const asdl::Attribute &attribute) const;
    void evaluate(const asdl::List &list) const;
    void evaluate(const asdl::Name &name) const;
    void evaluate(const asdl::Subscript &subscript) const;
    void evaluate(const asdl::Tuple &tuple) const;
    template <typename ASDL>
    [[noreturn]] void evaluate(const ASDL & /*asdl*/) const {
      throw object::BaseException(
          evaluator->builtins().get_attribute("RuntimeError"));
    }

  private:
    Evaluator *evaluator;
  };
} // namespace chimera::library::virtual_machine
