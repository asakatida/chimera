//! handles expressions like
//! a + b + c

#pragma once

#include "asdl/asdl.hpp"

#include <vector>

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
