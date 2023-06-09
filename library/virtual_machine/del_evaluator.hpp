// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! handles individual targets of delete expressions like
//! del a, b, c
//!     ^

#pragma once

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  struct DelEvaluator {
    explicit DelEvaluator(Evaluator *evaluator) noexcept;
    void evaluate(const asdl::Attribute &attribute) const;
    void evaluate(const asdl::Name &name) const;
    void evaluate(const asdl::Starred &starred) const;
    void evaluate(const asdl::Subscript &subscript) const;
    template <typename ASDL>
    [[noreturn]] void evaluate(const ASDL & /*asdl*/) const {
      throw object::BaseException(
          evaluator->builtins().get_attribute("RuntimeError"));
    }

  private:
    Evaluator *evaluator;
  };
} // namespace chimera::library::virtual_machine
