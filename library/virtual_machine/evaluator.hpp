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

//! handles statements and dispatches to other evaluators

#pragma once

#include <functional> // for function
#include <stack>
#include <variant> // for variant

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "virtual_machine/assign_evaluator.hpp"
#include "virtual_machine/bin_evaluator.hpp"
#include "virtual_machine/bool_evaluator.hpp"
#include "virtual_machine/call_evaluator.hpp"
#include "virtual_machine/push_stack.hpp"
#include "virtual_machine/thread_context.hpp"
#include "virtual_machine/to_bool_evaluator.hpp"
#include "virtual_machine/tuple_evaluator.hpp"
#include "virtual_machine/unary_evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct Evaluator;

      struct Scopes {
        explicit operator bool() const;

        object::Object &self();

        void enter_scope(const object::Object &main);

        void enter();

        void exit();

        void exit_scope();

        template <typename Instruction>
        void push(Instruction &&instruction) {
          scopes.top().bodies.top().steps.push(
              std::forward<Instruction>(instruction));
        }

        template <typename Visitor>
        void visit(Visitor &&visitor) {
          auto top = std::move(scopes.top().bodies.top().steps.top());
          scopes.top().bodies.top().steps.pop();
          std::visit(std::forward<Visitor>(visitor), std::move(top));
          while (true) {
            if (scopes.empty()) {
              break;
            }
            if (scopes.top().bodies.empty()) {
              scopes.pop();
              continue;
            }
            if (scopes.top().bodies.top().steps.empty()) {
              scopes.top().bodies.pop();
            }
          }
        }

      private:
        struct Scope {
          object::Object self;
          struct Body {
            using Step = std::variant<
                AssignEvaluator, BinAddEvaluator, BinSubEvaluator,
                BinMultEvaluator, BinMatMultEvaluator, BinDivEvaluator,
                BinModEvaluator, BinPowEvaluator, BinLShiftEvaluator,
                BinRShiftEvaluator, BinBitOrEvaluator, BinBitXorEvaluator,
                BinBitAndEvaluator, BinFloorDivEvaluator, BoolAndEvaluator,
                BoolOrEvaluator, CallEvaluator, PushStack, ToBoolEvaluator,
                TupleEvaluator, UnaryBitNotEvaluator, UnaryNotEvaluator,
                UnaryAddEvaluator, UnarySubEvaluator,
                std::function<void(Evaluator *)>>;
            std::stack<Step> steps{};
          };
          std::stack<Body> bodies{};
        };

        std::stack<Scope> scopes{};
      };

      struct Evaluator {
        ThreadContext &thread_context;
        Scopes scope{};
        std::stack<object::Object> stack{};

        object::Object &self();

        const object::Object &builtins() const;

        void enter_scope(const object::Object &object);

        void enter();

        void exit();

        void exit_scope();

        void extend(const std::vector<library::asdl::StmtImpl> &instructions);

        void extend(const std::vector<library::asdl::ExprImpl> &instructions);

        template <typename Instruction>
        void push(Instruction &&instruction) {
          scope.push(std::forward<Instruction>(instruction));
        }

        void evaluate(const library::asdl::StmtImpl &stmt);

        void evaluate_del(const library::asdl::ExprImpl &expr);

        void evaluate_get(const library::asdl::ExprImpl &expr);

        void evaluate_set(const library::asdl::ExprImpl &expr);

        void get_attribute(const object::Object &object,
                           const std::string &name);

        void evaluate();

        void evaluate(const library::asdl::Module &module);

        void evaluate(const library::asdl::Interactive &interactive);

        void evaluate(const library::asdl::Expression &expression);

        void evaluate(const library::asdl::FunctionDef &functionDef);
        void evaluate(const library::asdl::AsyncFunctionDef &asyncFunctionDef);
        void evaluate(const library::asdl::ClassDef &classDef);
        void evaluate(const library::asdl::Delete &asdlDelete);
        void evaluate(const library::asdl::Assign &assign);
        void evaluate(const library::asdl::AugAssign &augAssign);
        void evaluate(const library::asdl::AnnAssign &annAssign);
        void evaluate(const library::asdl::For &asdlFor);
        void evaluate(const library::asdl::AsyncFor &asyncFor);
        void evaluate(const library::asdl::While &asdlWhile);
        void evaluate(const library::asdl::If &asdlIf);
        void evaluate(const library::asdl::With &with);
        void evaluate(const library::asdl::AsyncWith &asyncWith);
        void evaluate(const library::asdl::Import &import);
        void evaluate(const library::asdl::ImportFrom &importFrom);
        void evaluate(const library::asdl::Global &global);
        void evaluate(const library::asdl::Nonlocal &nonlocal);
        void evaluate(const library::asdl::Expr &expr);
        void evaluate(const library::asdl::Raise &raise);
        void evaluate(const library::asdl::Try &asdlTry);
        void evaluate(const library::asdl::Assert &assert);
        void evaluate(const library::asdl::Return &asdlReturn);
        void evaluate(const library::asdl::Break &asdlBreak);
        void evaluate(const library::asdl::Continue &asdlContinue);

      private:
        std::optional<object::BaseException>
        do_try(const std::vector<library::asdl::StmtImpl> &body,
               const std::optional<object::BaseException> &context);

        void get_attribute(const object::Object &object,
                           const object::Object &getAttribute,
                           const std::string &name);

        void get_attr(const object::Object &object, const std::string &name);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
