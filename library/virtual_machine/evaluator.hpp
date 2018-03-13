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

//! handles statements and dispatches to other evaluators

#pragma once

#include <functional> // for function
#include <stack>
#include <variant> // for variant

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
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
          if (scopes.empty()) {
            return;
          }
          if (scopes.top().bodies.empty()) {
            exit_scope();
            return;
          }
          if (scopes.top().bodies.top().steps.empty()) {
            exit();
            return;
          }
          auto top = std::move(scopes.top().bodies.top().steps.top());
          scopes.top().bodies.top().steps.pop();
          std::visit(std::forward<Visitor>(visitor), std::move(top));
        }

      private:
        struct Scope {
          object::Object self;
          struct Body {
            using Step = std::variant<
                BinAddEvaluator, BinSubEvaluator, BinMultEvaluator,
                BinMatMultEvaluator, BinDivEvaluator, BinModEvaluator,
                BinPowEvaluator, BinLShiftEvaluator, BinRShiftEvaluator,
                BinBitOrEvaluator, BinBitXorEvaluator, BinBitAndEvaluator,
                BinFloorDivEvaluator, BoolAndEvaluator, BoolOrEvaluator,
                CallEvaluator, PushStack, ToBoolEvaluator, TupleEvaluator,
                UnaryBitNotEvaluator, UnaryNotEvaluator, UnaryAddEvaluator,
                UnarySubEvaluator, std::function<void(Evaluator *)>>;
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

        void extend(const std::vector<asdl::StmtImpl> &instructions);

        void extend(const std::vector<asdl::ExprImpl> &instructions);

        template <typename Instruction>
        void push(Instruction &&instruction) {
          scope.push(std::forward<Instruction>(instruction));
        }

        void evaluate(const asdl::StmtImpl &stmt);

        void evaluate_del(const asdl::ExprImpl &expr);

        void evaluate_get(const asdl::ExprImpl &expr);

        void evaluate_set(const asdl::ExprImpl &expr);

        void get_attribute(const object::Object &object,
                           const std::string &name);

        void evaluate();

        void evaluate(const asdl::Module &module);

        void evaluate(const asdl::Interactive &interactive);

        void evaluate(const asdl::Expression &expression);

        void evaluate(const asdl::FunctionDef &functionDef);
        void evaluate(const asdl::AsyncFunctionDef &asyncFunctionDef);
        void evaluate(const asdl::ClassDef &classDef);
        void evaluate(const asdl::Delete &asdlDelete);
        void evaluate(const asdl::Assign &assign);
        void evaluate(const asdl::AugAssign &augAssign);
        void evaluate(const asdl::AnnAssign &annAssign);
        void evaluate(const asdl::For &asdlFor);
        void evaluate(const asdl::AsyncFor &asyncFor);
        void evaluate(const asdl::While &asdlWhile);
        void evaluate(const asdl::If &asdlIf);
        void evaluate(const asdl::With &with);
        void evaluate(const asdl::AsyncWith &asyncWith);
        void evaluate(const asdl::Import &import);
        void evaluate(const asdl::ImportFrom &importFrom);
        void evaluate(const asdl::Global &global);
        void evaluate(const asdl::Nonlocal &nonlocal);
        void evaluate(const asdl::Expr &expr);
        void evaluate(const asdl::Raise &raise);
        void evaluate(const asdl::Try &asdlTry);
        void evaluate(const asdl::Assert &assert);
        void evaluate(const asdl::Return &asdlReturn);
        void evaluate(const asdl::Break &asdlBreak);
        void evaluate(const asdl::Continue &asdlContinue);

      private:
        std::optional<object::BaseException>
        do_try(const std::vector<asdl::StmtImpl> &body,
               const std::optional<object::BaseException> &context);

        void get_attribute(const object::Object &object,
                           const object::Object &getAttribute,
                           const std::string &name);

        void get_attr(const object::Object &object, const std::string &name);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
