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

//! handles expressions like
//! a + b + c

#pragma once

#include <vector>

#include "asdl/asdl.hpp"

namespace chimera::library::virtual_machine {
  struct Evaluator;

  struct BinAddEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinSubEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinMultEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinMatMultEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinDivEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinModEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinPowEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinLShiftEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinRShiftEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinBitOrEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinBitXorEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinBitAndEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
  struct BinFloorDivEvaluator {
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    Iterator begin;
    Iterator end;

    void operator()(Evaluator *evaluatorA) const;
  };
} // namespace chimera::library::virtual_machine
