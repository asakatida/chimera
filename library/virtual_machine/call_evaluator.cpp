// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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
//! a(b, c=None)

#include "virtual_machine/call_evaluator.hpp"

#include <gsl/gsl>

#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct UnpackCallObject {
        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::Instance & /*instance*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::Bytes & /*bytes*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::BytesMethod & /*bytesMethod*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::False & /*false*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::Future & /*future*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::None & /*none*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::NullFunction & /*nullFunction*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::Number & /*number*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::NumberMethod & /*numberMethod*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::ObjectMethod & /*objectMethod*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::String & /*string*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::StringMethod & /*stringMethod*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::SysCall & /*sysCall*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::True & /*true*/) const {
          Expects(false);
        }

        [[noreturn]] void evaluate(Evaluator * /*evaluator*/,
                                   const object::Tuple & /*tuple*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const object::TupleMethod & /*tupleMethod*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const library::asdl::StmtImpl & /*stmtImpl*/) const {
          Expects(false);
        }

        [[noreturn]] void
        evaluate(Evaluator * /*evaluator*/,
                 const library::asdl::ExprImpl & /*exprImpl*/) const {
          Expects(false);
        }

        void operator()(Evaluator *evaluator) const {
          auto top = std::move(evaluator->stack.top());
          evaluator->stack.pop();
          std::visit(
              [this, evaluator](const auto &value) {
                this->evaluate(evaluator, value);
              },
              top.value());
        }
      };

      void CallEvaluator::operator()(Evaluator *evaluatorA) const {
        if (std::holds_alternative<object::Instance>(object.value())) {
          evaluatorA->push([](Evaluator *evaluatorB) {
            auto top = std::move(evaluatorB->stack.top());
            evaluatorB->stack.pop();
          });
          evaluatorA->get_attribute(object, "__call__");
        } else {
          evaluatorA->push(UnpackCallObject{});
        }
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
