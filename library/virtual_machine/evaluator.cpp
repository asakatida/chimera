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
//! all actions defer work

#include "virtual_machine/evaluator.hpp"

#include <exception>
#include <istream>
#include <string_view>

#include <gsl/gsl>

#include "asdl/asdl.hpp"
#include "container/reverse.hpp"
#include "virtual_machine/del_evaluator.hpp"
#include "virtual_machine/get_evaluator.hpp"
#include "virtual_machine/set_evaluator.hpp"

using namespace std::literals;

namespace chimera::library::virtual_machine {
  class ReRaise final : std::exception {
    [[nodiscard]] auto what() const noexcept -> const char * override;
  };
  auto ReRaise::what() const noexcept -> const char * { return "ReRaise"; }
  Scopes::operator bool() const { return !scopes.empty(); }
  auto Scopes::self() -> object::Object & { return scopes.top().self; }
  void Scopes::enter_scope(const object::Object &main) {
    scopes.emplace(Scope{main});
    enter();
  }
  void Scopes::enter() { scopes.top().bodies.emplace(); }
  void Scopes::exit() { scopes.top().bodies.pop(); }
  void Scopes::exit_scope() { scopes.pop(); }
  Evaluator::Evaluator(ThreadContext &thread_context) noexcept
      : thread_context(thread_context) {}
  auto Evaluator::self() -> object::Object & { return scope.self(); }
  auto Evaluator::builtins() const -> const object::Object & {
    return thread_context.builtins();
  }
  void Evaluator::enter_scope(const object::Object &object) {
    scope.enter_scope(object);
  }
  void Evaluator::enter() { scope.enter(); }
  void Evaluator::exit() { scope.exit(); }
  void Evaluator::exit_scope() { scope.exit_scope(); }
  void Evaluator::extend(const std::vector<asdl::StmtImpl> &instructions) {
    if (instructions.empty()) {
      return;
    }
    for (const auto &instruction : container::reverse(instructions)) {
      evaluate(instruction);
    }
  }
  void Evaluator::extend(const std::vector<asdl::ExprImpl> &instructions) {
    for (const auto &instruction : container::reverse(instructions)) {
      evaluate_get(instruction);
    }
  }
  auto Evaluator::return_value() const -> object::Object {
    return thread_context.return_value();
  }
  void Evaluator::stack_pop() { stack.pop(); }
  void Evaluator::stack_push(const object::Object &object) {
    stack.push(object);
  }
  auto Evaluator::stack_remove() -> object::Object {
    auto finally = gsl::finally([this] { this->stack.pop(); });
    return std::move(stack.top());
  }
  auto Evaluator::stack_size() const -> std::size_t { return stack.size(); }
  auto Evaluator::stack_top() const -> const object::Object & {
    return stack.top();
  }
  void Evaluator::stack_top_update(const object::Object &object) {
    stack.top() = object;
  }
  void Evaluator::evaluate(const asdl::StmtImpl &stmt) {
    stmt.visit([this](auto &&value) { this->evaluate(value); });
  }
  void Evaluator::evaluate_del(const asdl::ExprImpl &expr) {
    expr.visit([this](auto &&value) { DelEvaluator{this}.evaluate(value); });
  }
  void Evaluator::evaluate_get(const asdl::ExprImpl &expr) {
    expr.visit([this](auto &&value) { GetEvaluator{this}.evaluate(value); });
  }
  void Evaluator::evaluate_set(const asdl::ExprImpl &expr) {
    expr.visit([this](auto &&value) { SetEvaluator{this}.evaluate(value); });
  }
  void Evaluator::get_attribute(const object::Object &object,
                                const std::string &name) {
    const std::string getAttribute("__getattribute__");
    if (object.has_attribute(getAttribute)) {
      return get_attribute(object, object.get_attribute(getAttribute), name);
    }
    auto mro = object.get_attribute("__class__").get_attribute("__mro__");
    if (const auto tuple = mro.get<object::Tuple>()) {
      for (const auto &type : *tuple) {
        if (type.has_attribute(getAttribute)) {
          return get_attribute(object, type.get_attribute(getAttribute), name);
        }
      }
    }
    throw object::BaseException(builtins().get_attribute("AttributeError"));
  }
  void Evaluator::evaluate() {
    while (scope) {
      thread_context.process_interrupts();
      //! where all defered work gets done
      scope.visit([this](auto &&value) { value(this); });
    }
  }
  void Evaluator::evaluate(const asdl::Module &module) {
    enter_scope(thread_context.body());
    if (const auto &doc_string = module.doc(); doc_string) {
      self().set_attribute("__doc__"s, doc_string->string);
    } else {
      self().set_attribute("__doc__"s, builtins().get_attribute("None"));
    }
    extend(module.iter());
    return evaluate();
  }
  void Evaluator::evaluate(const asdl::Interactive &interactive) {
    enter_scope(thread_context.body());
    extend(interactive.iter());
    return evaluate();
  }
  void Evaluator::evaluate(const asdl::Expression &expression) {
    enter_scope(thread_context.body());
    push([](Evaluator *evaluator) {
      evaluator->thread_context.return_value(evaluator->stack_remove());
    });
    evaluate_get(expression.expr());
    return evaluate();
  }
  void Evaluator::evaluate(const asdl::FunctionDef &functionDef) {
    for (const auto &expr : container::reverse(functionDef.decorator_list)) {
      push([](Evaluator *evaluator) {
        auto decorator = evaluator->stack_remove();
        evaluator->push(CallEvaluator{decorator, {evaluator->stack_top()}});
      });
      evaluate_get(expr);
    }
    if (functionDef.returns) {
    }
    for (const auto &arg : container::reverse(functionDef.args.args)) {
      // arg.name;
      if (arg.annotation) {
      }
      if (arg.arg_default) {
      }
    }
    if (functionDef.args.vararg) {
    }
    for (const auto &kwarg : container::reverse(functionDef.args.kwonlyargs)) {
      // kwarg.name;
      if (kwarg.annotation) {
      }
      if (kwarg.arg_default) {
      }
    }
    if (functionDef.args.kwarg) {
    }
    if (functionDef.doc_string) {
      push([&functionDef](Evaluator *evaluator) {
        auto top = evaluator->stack_top();
        top.set_attribute("__doc__"s, functionDef.doc_string->string);
      });
    }
    push(PushStack{object::Object(
        {{"__doc__", builtins().get_attribute("None")},
         {"__name__",
          object::Object(object::String(functionDef.name.value), {})},
         {"__qualname__",
          object::Object(object::String(functionDef.name.value), {})},
         {"__module__", thread_context.body().get_attribute("__name__")},
         {"__defaults__", builtins().get_attribute("None")},
         {"__code__", {}},
         {"__globals__", thread_context.body()},
         {"__closure__", self()},
         {"__annotations__", {}},
         {"__kwdefaults__", {}}})});
  }
  void
  Evaluator::evaluate(const asdl::AsyncFunctionDef & /*async_function_def*/) {}
  void Evaluator::evaluate(const asdl::ClassDef & /*class_def*/) {}
  void Evaluator::evaluate(const asdl::Delete &asdlDelete) {
    for (const auto &target : container::reverse(asdlDelete.targets)) {
      evaluate_del(target);
    }
  }
  void Evaluator::evaluate(const asdl::Assign &assign) {
    for (const auto &expr : container::reverse(assign.targets)) {
      evaluate_set(expr);
    }
    evaluate_get(assign.value);
  }
  void Evaluator::evaluate(const asdl::AugAssign & /*aug_assign*/) {}
  void Evaluator::evaluate(const asdl::AnnAssign & /*ann_assign*/) {}
  void Evaluator::evaluate(const asdl::For &asdlFor) {
    push([&asdlFor](Evaluator *evaluatorA) {
      try {
        Evaluator evaluatorB{evaluatorA->thread_context};
        evaluatorB.enter_scope(evaluatorA->self());
        evaluatorB.push([](Evaluator *evaluatorC) {
          evaluatorC->push(CallEvaluator{evaluatorC->stack_remove()});
        });
        evaluatorB.get_attribute(evaluatorA->stack_top(), "__next__");
        evaluatorB.evaluate();
      } catch (const object::BaseException &error) {
        if (error.class_id() ==
            evaluatorA->builtins().get_attribute("StopIteration").id()) {
          evaluatorA->exit();
          evaluatorA->extend(asdlFor.orelse);
        } else {
          throw error;
        }
      }
      evaluatorA->enter();
      evaluatorA->push([&asdlFor](Evaluator *evaluatorB) {
        evaluatorB->exit();
        evaluatorB->evaluate(asdlFor);
      });
      evaluatorA->extend(asdlFor.body);
      evaluatorA->evaluate_set(asdlFor.target);
    });
    push([](Evaluator *evaluatorA) {
      evaluatorA->push(CallEvaluator{evaluatorA->stack_remove()});
    });
    push([](Evaluator *evaluatorA) {
      evaluatorA->get_attribute(evaluatorA->stack_top(), "__iter__");
      evaluatorA->stack_pop();
    });
    evaluate_get(asdlFor.iter);
    push([](Evaluator *evaluatorA) { evaluatorA->enter(); });
  }
  void Evaluator::evaluate(const asdl::AsyncFor & /*async_for*/) {}
  void Evaluator::evaluate(const asdl::While &asdlWhile) {
    push([&asdlWhile](Evaluator *evaluatorA) {
      if (evaluatorA->stack_top().get_bool()) {
        evaluatorA->enter();
        evaluatorA->push([&asdlWhile](Evaluator *evaluatorB) {
          evaluatorB->exit();
          evaluatorB->evaluate(asdlWhile);
        });
        evaluatorA->extend(asdlWhile.body);
      } else {
        evaluatorA->exit();
        evaluatorA->extend(asdlWhile.orelse);
      }
      evaluatorA->stack_pop();
    });
    push([](Evaluator *evaluatorA) {
      evaluatorA->push(ToBoolEvaluator{evaluatorA->stack_top()});
      evaluatorA->stack_pop();
    });
    evaluate_get(asdlWhile.test);
    push([](Evaluator *evaluatorA) { evaluatorA->enter(); });
  }
  void Evaluator::evaluate(const asdl::If & /*asdlIf*/) {
    // push([&asdlIf](Evaluator *evaluator) {
    //   if (evaluator->stack_top().get_bool()) {
    //     evaluator->extend(asdlIf.body);
    //   } else {
    //     evaluator->extend(asdlIf.orelse);
    //   }
    //   evaluator->stack_pop();
    // });
    // push([](Evaluator *evaluator) {
    //   evaluator->push(ToBoolEvaluator{evaluator->stack_top()});
    //   evaluator->stack_pop();
    // });
    // evaluate_get(asdlIf.test);
  }
  void Evaluator::evaluate(const asdl::With &with) {
    push([&with](Evaluator *evaluator) {
      if (auto exception = evaluator->do_try(with.body, {}); exception) {
        evaluator->do_try(with.body, exception);
      } else {
        evaluator->do_try(with.body, {});
      }
    });
    for (const auto &withItem : container::reverse(with.items)) {
      evaluate_get(withItem.context_expr);
    }
  }
  void Evaluator::evaluate(const asdl::AsyncWith & /*async_with*/) {}
  void Evaluator::evaluate(const asdl::Import &import) {
    for (const auto &alias : container::reverse(import.names)) {
      if (alias.asname) {
        push([&alias](Evaluator *evaluator) {
          evaluator->self().set_attribute(alias.asname->value,
                                          evaluator->stack_remove());
          evaluator->stack_pop();
        });
      } else {
        push([&alias](Evaluator *evaluator) {
          evaluator->self().set_attribute(alias.name.value,
                                          evaluator->stack_remove());
          evaluator->stack_pop();
        });
      }
      push([&alias](Evaluator *evaluator) {
        evaluator->push(PushStack{evaluator->thread_context.import_object(
            "module_name"sv, alias.name.value)});
      });
    }
  }
  void Evaluator::evaluate(const asdl::ImportFrom &importFrom) {
    push([](Evaluator *evaluator) { evaluator->stack_pop(); });
    for (const auto &alias : container::reverse(importFrom.names)) {
      if (alias.asname) {
        push([&alias](Evaluator *evaluator) {
          evaluator->self().set_attribute(
              alias.asname->value,
              evaluator->stack_top().get_attribute(alias.name.value));
        });
      } else {
        push([&alias](Evaluator *evaluator) {
          evaluator->self().set_attribute(
              alias.name.value,
              evaluator->stack_top().get_attribute(alias.name.value));
        });
      }
    }
    push([&importFrom](Evaluator *evaluator) {
      evaluator->push(PushStack{evaluator->thread_context.import_object(
          "module_name"sv, importFrom.module.value)});
    });
  }
  void Evaluator::evaluate(const asdl::Global & /*global*/) {}
  void Evaluator::evaluate(const asdl::Nonlocal & /*nonlocal*/) {}
  void Evaluator::evaluate(const asdl::Expr &expr) {
    push([](Evaluator *evaluator) { evaluator->stack_pop(); });
    evaluate_get(expr.value);
  }
  void Evaluator::evaluate(const asdl::Raise &raise) {
    if (raise.exc) {
      if (raise.cause) {
        push([](Evaluator *evaluator) {
          const auto cause = object::BaseException(evaluator->stack_remove());
          const auto exception = object::BaseException(evaluator->stack_top());
          throw object::BaseException(exception, cause);
        });
        evaluate_get(*raise.cause);
      } else {
        push([](Evaluator *evaluator) {
          throw object::BaseException(evaluator->stack_top());
        });
      }
      evaluate_get(*raise.exc);
    } else {
      push([](Evaluator * /*evaluator*/) { throw ReRaise{}; });
    }
  }
  void Evaluator::evaluate(const asdl::Try &asdlTry) {
    push([](Evaluator *evaluator) {
      if (evaluator->thread_context.return_value().get_bool()) {
        evaluator->exit_scope();
      }
    });
    if (auto exception = do_try(asdlTry.body, {}); exception) {
      for (const auto &handler : container::reverse(asdlTry.handlers)) {
        std::visit([](auto &&) {}, handler);
      }
      if (auto exc = do_try(asdlTry.finalbody, exception); exc) {
        throw object::BaseException(*exc, *exception);
      }
      throw object::BaseException(*exception);
    }
    if (auto exception = do_try(asdlTry.orelse, {}); exception) {
      if (auto exc = do_try(asdlTry.finalbody, exception); exc) {
        throw object::BaseException(*exc, *exception);
      }
      throw object::BaseException(*exception);
    }
    extend(asdlTry.finalbody);
  }
  void Evaluator::evaluate(const asdl::Assert &assert) {
    if (builtins().get_attribute("__debug__").get_bool()) {
      push([&assert](Evaluator *evaluatorA) {
        if (!evaluatorA->stack_top().get_bool()) {
          return evaluatorA->stack_pop();
        }
        evaluatorA->stack_pop();
        if (!assert.msg) {
          throw object::BaseException(
              evaluatorA->builtins().get_attribute("AssertionError"));
        }
        evaluatorA->push([](Evaluator *evaluatorB) {
          throw object::BaseException(
              evaluatorB->builtins().get_attribute("AssertionError"));
        });
        evaluatorA->evaluate_get(*assert.msg);
      });
      evaluate_get(assert.test);
    }
  }
  void Evaluator::evaluate(const asdl::Return &asdlReturn) {
    if (asdlReturn.value) {
      push([](Evaluator *evaluator) {
        evaluator->thread_context.return_value(evaluator->stack_remove());
        evaluator->exit_scope();
      });
      evaluate_get(*asdlReturn.value);
    } else {
      exit_scope();
    }
  }
  void Evaluator::evaluate(const asdl::Break & /*break*/) {
    push([](Evaluator *evaluator) {
      evaluator->exit();
      evaluator->exit();
    });
  }
  void Evaluator::evaluate(const asdl::Continue & /*continue*/) {
    push([](Evaluator *evaluator) { evaluator->exit(); });
  }
  auto Evaluator::do_try(const std::vector<asdl::StmtImpl> &body,
                         const std::optional<object::BaseException> &context)
      -> std::optional<object::BaseException> {
    try {
      Evaluator evaluator{thread_context};
      evaluator.enter_scope(self());
      evaluator.extend(body);
      evaluator.evaluate();
    } catch (const object::BaseException &error) {
      if (context) {
        return object::BaseException(error, *context);
      }
      return error;
    } catch (const ReRaise &) {
      if (context) {
        return context;
      }
      return object::BaseException(builtins().get_attribute("RuntimeError"));
    } catch (const std::exception &exc) {
      auto exception = object::Object{object::String{exc.what()}, {}};
      object::BaseException error(exception);
      if (context) {
        return object::BaseException(error, *context);
      }
      return error;
    }
    return {};
  }
  void Evaluator::get_attribute(const object::Object &object,
                                const object::Object &getAttribute,
                                const std::string &name) {
    if (getAttribute.get<object::ObjectMethod>() ==
        object::ObjectMethod::GETATTRIBUTE) {
      if (getAttribute.get_attribute("__class__").id() == 0 /* method */) {
        if (object.has_attribute(name)) {
          return push(PushStack{object.get_attribute(name)});
        }
        auto mro = object.get_attribute("__class__").get_attribute("__mro__");
        if (const auto tuple = mro.get<object::Tuple>()) {
          for (const auto &type : *tuple) {
            if (type.has_attribute(name)) {
              return push(PushStack{type.get_attribute(name)});
            }
          }
        }
        return get_attr(object, name);
      }
    }
    push(CallEvaluator{
        getAttribute,
        {object::Object(object::String(name),
                        {{"__class__", builtins().get_attribute("str")}})}});
  }
  void Evaluator::get_attr(const object::Object &object,
                           const std::string &name) {
    push([name](Evaluator *evaluator) {
      evaluator->push(CallEvaluator{
          evaluator->stack_remove(),
          {object::Object(
              object::String(name),
              {{"__class__", evaluator->builtins().get_attribute("str")}})}});
    });
    if (object.has_attribute("__getattr__")) {
      return push(PushStack{object.get_attribute("__getattr__")});
    }
    auto mro = object.get_attribute("__class__").get_attribute("__mro__");
    if (const auto tuple = mro.get<object::Tuple>()) {
      for (const auto &type : *tuple) {
        if (type.has_attribute("__getattr__")) {
          return push(PushStack{type.get_attribute("__getattr__")});
        }
      }
    }
    throw object::BaseException(builtins().get_attribute("AttributeError"));
  }
} // namespace chimera::library::virtual_machine
