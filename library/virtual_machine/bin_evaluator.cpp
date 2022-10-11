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

#include "virtual_machine/bin_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  void BinAddEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinAddEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__add__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinSubEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinSubEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__sub__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinMultEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinMultEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__mul__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinMatMultEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinMatMultEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__matmul__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinDivEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinDivEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__div__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinModEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinModEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__mod__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinPowEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinPowEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__pow__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinLShiftEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinLShiftEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__lshift__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinRShiftEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinRShiftEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__rshift__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinBitOrEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitOrEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__or__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinBitXorEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitXorEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__xor__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinBitAndEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinBitAndEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__and__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
  void BinFloorDivEvaluator::operator()(Evaluator *evaluatorA) const {
    if (begin != end) {
      const auto &expr = *begin;
      evaluatorA->push(BinFloorDivEvaluator{begin + 1, end});
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->push(CallEvaluator{evaluatorB->stack_top(), {}, {}});
        evaluatorB->stack_pop();
      });
      evaluatorA->push([](Evaluator *evaluatorB) {
        evaluatorB->get_attribute(evaluatorB->stack_top(), "__floordiv__");
        evaluatorB->stack_pop();
      });
      evaluatorA->evaluate_get(expr);
    }
  }
} // namespace chimera::library::virtual_machine
