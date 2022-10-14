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
    explicit BinAddEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinAddEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinSubEvaluator {
    explicit BinSubEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinSubEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinMultEvaluator {
    explicit BinMultEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinMultEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinMatMultEvaluator {
    explicit BinMatMultEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinMatMultEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinDivEvaluator {
    explicit BinDivEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinDivEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinModEvaluator {
    explicit BinModEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinModEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinPowEvaluator {
    explicit BinPowEvaluator(const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinPowEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinLShiftEvaluator {
    explicit BinLShiftEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinLShiftEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinRShiftEvaluator {
    explicit BinRShiftEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinRShiftEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinBitOrEvaluator {
    explicit BinBitOrEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinBitOrEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinBitXorEvaluator {
    explicit BinBitXorEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinBitXorEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinBitAndEvaluator {
    explicit BinBitAndEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinBitAndEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
  struct BinFloorDivEvaluator {
    explicit BinFloorDivEvaluator(
        const std::vector<asdl::ExprImpl> &exprs) noexcept;
    void operator()(Evaluator *evaluator) const;

  private:
    using Iterator = std::vector<asdl::ExprImpl>::const_iterator;
    BinFloorDivEvaluator(const Iterator &begin, const Iterator &end) noexcept;
    Iterator begin;
    Iterator end;
  };
} // namespace chimera::library::virtual_machine
