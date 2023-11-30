//! handles expressions like
//! a + b + c

#include "virtual_machine/bin_evaluator.hpp"

#include "asdl/asdl.hpp" // for ExprImpl
#include "virtual_machine/call_evaluator.hpp"
#include "virtual_machine/evaluator.hpp"

#include <vector> // for vector

namespace chimera::library::virtual_machine {
  BinAddEvaluator::BinAddEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinAddEvaluator(exprs.begin(), exprs.end()) {}
  BinAddEvaluator::BinAddEvaluator(
      const BinAddEvaluator::Iterator &begin,
      const BinAddEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinAddEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinAddEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__add__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinSubEvaluator::BinSubEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinSubEvaluator(exprs.begin(), exprs.end()) {}
  BinSubEvaluator::BinSubEvaluator(
      const BinSubEvaluator::Iterator &begin,
      const BinSubEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinSubEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinSubEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__sub__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinMultEvaluator::BinMultEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinMultEvaluator(exprs.begin(), exprs.end()) {}
  BinMultEvaluator::BinMultEvaluator(
      const BinMultEvaluator::Iterator &begin,
      const BinMultEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinMultEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinMultEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__mul__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinMatMultEvaluator::BinMatMultEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinMatMultEvaluator(exprs.begin(), exprs.end()) {}
  BinMatMultEvaluator::BinMatMultEvaluator(
      const BinMatMultEvaluator::Iterator &begin,
      const BinMatMultEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinMatMultEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinMatMultEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__matmul__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinDivEvaluator::BinDivEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinDivEvaluator(exprs.begin(), exprs.end()) {}
  BinDivEvaluator::BinDivEvaluator(
      const BinDivEvaluator::Iterator &begin,
      const BinDivEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinDivEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinDivEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__div__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinModEvaluator::BinModEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinModEvaluator(exprs.begin(), exprs.end()) {}
  BinModEvaluator::BinModEvaluator(
      const BinModEvaluator::Iterator &begin,
      const BinModEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinModEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinModEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__mod__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinPowEvaluator::BinPowEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinPowEvaluator(exprs.begin(), exprs.end()) {}
  BinPowEvaluator::BinPowEvaluator(
      const BinPowEvaluator::Iterator &begin,
      const BinPowEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinPowEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinPowEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__pow__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinLShiftEvaluator::BinLShiftEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinLShiftEvaluator(exprs.begin(), exprs.end()) {}
  BinLShiftEvaluator::BinLShiftEvaluator(
      const BinLShiftEvaluator::Iterator &begin,
      const BinLShiftEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinLShiftEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinLShiftEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__lshift__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinRShiftEvaluator::BinRShiftEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinRShiftEvaluator(exprs.begin(), exprs.end()) {}
  BinRShiftEvaluator::BinRShiftEvaluator(
      const BinRShiftEvaluator::Iterator &begin,
      const BinRShiftEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinRShiftEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinRShiftEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__rshift__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinBitOrEvaluator::BinBitOrEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinBitOrEvaluator(exprs.begin(), exprs.end()) {}
  BinBitOrEvaluator::BinBitOrEvaluator(
      const BinBitOrEvaluator::Iterator &begin,
      const BinBitOrEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinBitOrEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitOrEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__or__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinBitXorEvaluator::BinBitXorEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinBitXorEvaluator(exprs.begin(), exprs.end()) {}
  BinBitXorEvaluator::BinBitXorEvaluator(
      const BinBitXorEvaluator::Iterator &begin,
      const BinBitXorEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinBitXorEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitXorEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__xor__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinBitAndEvaluator::BinBitAndEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinBitAndEvaluator(exprs.begin(), exprs.end()) {}
  BinBitAndEvaluator::BinBitAndEvaluator(
      const BinBitAndEvaluator::Iterator &begin,
      const BinBitAndEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinBitAndEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitAndEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__and__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  BinFloorDivEvaluator::BinFloorDivEvaluator(
      const std::vector<asdl::ExprImpl> &exprs) noexcept
      : BinFloorDivEvaluator(exprs.begin(), exprs.end()) {}
  BinFloorDivEvaluator::BinFloorDivEvaluator(
      const BinFloorDivEvaluator::Iterator &begin,
      const BinFloorDivEvaluator::Iterator &end) noexcept
      : begin(begin), end(end) {}
  void BinFloorDivEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinFloorDivEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_remove()});
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__floordiv__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
} // namespace chimera::library::virtual_machine
