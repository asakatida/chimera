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

#include <functional>
#include <stack>
#include <variant>

#include "asdl/asdl.hpp"
#include "virtual_machine/bin_evaluator.hpp"
#include "virtual_machine/bool_evaluator.hpp"
#include "virtual_machine/call_evaluator.hpp"
#include "virtual_machine/push_stack.hpp"
#include "virtual_machine/thread_context.hpp"
#include "virtual_machine/to_bool_evaluator.hpp"
#include "virtual_machine/tuple_evaluator.hpp"
#include "virtual_machine/unary_evaluator.hpp"

namespace chimera::library::virtual_machine {
  struct Evaluator;
  struct Scopes {
    explicit operator bool() const;
    auto self() -> object::Object &;
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
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    ThreadContext &thread_context;
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    Scopes scope{};
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    std::stack<object::Object> stack{};
    auto self() -> object::Object &;
    [[nodiscard]] auto builtins() const -> const object::Object &;
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
    void get_attribute(const object::Object &object, const std::string &name);
    void evaluate();
    void evaluate(const asdl::Module &module);
    void evaluate(const asdl::Interactive &interactive);
    void evaluate(const asdl::Expression &expression);
    void evaluate(const asdl::AnnAssign &annAssign);
    void evaluate(const asdl::Assert &assert);
    void evaluate(const asdl::Assign &assign);
    void evaluate(const asdl::AsyncFor &asyncFor);
    void evaluate(const asdl::AsyncFunctionDef &asyncFunctionDef);
    void evaluate(const asdl::AsyncWith &asyncWith);
    void evaluate(const asdl::Attribute &attribute);
    void evaluate(const asdl::AugAssign &augAssign);
    void evaluate(const asdl::Await &await);
    void evaluate(const asdl::Bin &bin);
    void evaluate(const asdl::Bool &asdlBool);
    void evaluate(const asdl::Break &asdlBreak);
    void evaluate(const asdl::Call &call);
    void evaluate(const asdl::ClassDef &classDef);
    void evaluate(const asdl::Compare &compare);
    void evaluate(const asdl::Continue &asdlContinue);
    void evaluate(const asdl::Delete &asdlDelete);
    void evaluate(const asdl::Dict &dict);
    void evaluate(const asdl::DictComp &dictComp);
    void evaluate(const asdl::Ellipsis &ellipsis);
    void evaluate(const asdl::Expr &expr);
    void evaluate(const asdl::ExprImpl &expr);
    void evaluate(const asdl::For &asdlFor);
    void evaluate(const asdl::FormattedValue &formattedValue);
    void evaluate(const asdl::FunctionDef &functionDef);
    void evaluate(const asdl::GeneratorExp &generatorExp);
    void evaluate(const asdl::Global &global);
    void evaluate(const asdl::If &asdlIf);
    void evaluate(const asdl::IfExp &ifExp);
    void evaluate(const asdl::Import &import);
    void evaluate(const asdl::ImportFrom &importFrom);
    void evaluate(const asdl::JoinedStr &joinedStr);
    void evaluate(const asdl::Lambda &lambda);
    void evaluate(const asdl::List &list);
    void evaluate(const asdl::ListComp &listComp);
    void evaluate(const asdl::Name &name);
    void evaluate(const asdl::NameConstant &nameConstant);
    void evaluate(const asdl::Nonlocal &nonlocal);
    void evaluate(const asdl::Raise &raise);
    void evaluate(const asdl::Return &asdlReturn);
    void evaluate(const asdl::Set &set);
    void evaluate(const asdl::SetComp &setComp);
    void evaluate(const asdl::Starred &starred);
    void evaluate(const asdl::Subscript &subscript);
    void evaluate(const asdl::Try &asdlTry);
    void evaluate(const asdl::Tuple &tuple);
    void evaluate(const asdl::Unary &unary);
    void evaluate(const asdl::UnpackDict &unpackDict);
    void evaluate(const asdl::While &asdlWhile);
    void evaluate(const asdl::With &with);
    void evaluate(const asdl::Yield &yield);
    void evaluate(const asdl::YieldFrom &yieldFrom);
    void evaluate(const object::Object &object);

  private:
    auto do_try(const std::vector<asdl::StmtImpl> &body,
                const std::optional<object::BaseException> &context)
        -> std::optional<object::BaseException>;
    void get_attribute(const object::Object &object,
                       const object::Object &getAttribute,
                       const std::string &name);
    void get_attr(const object::Object &object, const std::string &name);
  };
} // namespace chimera::library::virtual_machine
