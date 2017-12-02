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

#include <algorithm>
#include <atomic>
#include <functional> // for function
#include <iostream>
#include <stack>
#include <string>
#include <string_view>
#include <variant> // for variant

#include <gsl/gsl>

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "options.hpp"
#include "reverse.hpp"
#include "virtual_machine/del_evaluator.hpp"
#include "virtual_machine/get_evaluator.hpp"
#include "virtual_machine/set_evaluator.hpp"
#include "virtual_machine/slice_evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct ReRaise final : std::exception {
        const char *what() const noexcept override;
      };
      const char *ReRaise::what() const noexcept { return "ReRaise"; }

      Scopes::operator bool() const { return !scopes.empty(); }

      object::Object &Scopes::self() { return scopes.top().self; }

      void Scopes::enter_scope(const object::Object &main) {
        scopes.emplace(Scope{main});
        enter();
      }

      void Scopes::enter() { scopes.top().bodies.emplace(); }

      void Scopes::exit() { scopes.top().bodies.pop(); }

      void Scopes::exit_scope() { scopes.pop(); }

      object::Object &Evaluator::self() { return scope.self(); }

      const object::Object &Evaluator::builtins() const {
        return thread_context.process_context.global_context.builtins;
      }

      void Evaluator::enter_scope(const object::Object &object) {
        scope.enter_scope(object);
      }

      void Evaluator::enter() { scope.enter(); }

      void Evaluator::exit() { scope.exit(); }

      void Evaluator::exit_scope() { scope.exit_scope(); }

      void Evaluator::extend(
          const std::vector<library::asdl::StmtImpl> &instructions) {
        for (auto &&instruction : container::reverse(instructions)) {
          evaluate(instruction);
        }
      }

      void Evaluator::extend(
          const std::vector<library::asdl::ExprImpl> &instructions) {
        for (auto &&instruction : container::reverse(instructions)) {
          evaluate_get(instruction);
        }
      }

      void Evaluator::evaluate(const library::asdl::StmtImpl &stmt) {
        std::visit([this](auto &&value) { this->evaluate(value); },
                   *stmt.value);
      }

      void Evaluator::evaluate_del(const library::asdl::ExprImpl &expr) {
        std::visit([this](auto &&value) { DelEvaluator{this}.evaluate(value); },
                   *expr.value);
      }

      void Evaluator::evaluate_get(const library::asdl::ExprImpl &expr) {
        std::visit([this](auto &&value) { GetEvaluator{this}.evaluate(value); },
                   *expr.value);
      }

      void Evaluator::evaluate_set(const library::asdl::ExprImpl &expr) {
        std::visit([this](auto &&value) { SetEvaluator{this}.evaluate(value); },
                   *expr.value);
      }

      void Evaluator::get_attribute(const object::Object &object,
                                    const std::string &name) {
        std::string getAttribute("__getattribute__");
        if (object.has_attribute(getAttribute)) {
          return get_attribute(object, object.get_attribute(getAttribute),
                               name);
        }
        auto mro = object.get_attribute("__class__").get_attribute("__mro__");
        for (auto &&type : std::get<object::Tuple>(mro.value())) {
          if (type.has_attribute(getAttribute)) {
            return get_attribute(object, type.get_attribute(getAttribute),
                                 name);
          }
        }
        Ensures(false);
      }

      void Evaluator::evaluate() {
        while (scope) {
          if (!std::atomic_flag_test_and_set(
                  thread_context.process_context.global_context.sig_int)) {
            throw object::BaseException{
                builtins().get_attribute("KeyboardInterrupt")};
          }
          //! where all defered work gets done
          scope.visit([this](auto &&value) { value(this); });
        }
      }

      void Evaluator::evaluate(const library::asdl::Module &module) {
        enter_scope(thread_context.main);
        extend(module.body);
        return evaluate();
      }

      void Evaluator::evaluate(const library::asdl::Interactive &interactive) {
        enter_scope(thread_context.main);
        extend(interactive.body);
        return evaluate();
      }

      void Evaluator::evaluate(const library::asdl::Expression &expression) {
        enter_scope(thread_context.main);
        evaluate_get(expression.body);
        return evaluate();
      }

      void
      Evaluator::evaluate(const library::asdl::FunctionDef & /*function_def*/) {
      }
      void Evaluator::evaluate(
          const library::asdl::AsyncFunctionDef & /*async_function_def*/) {}
      void Evaluator::evaluate(const library::asdl::ClassDef & /*class_def*/) {}
      void Evaluator::evaluate(const library::asdl::Delete &asdlDelete) {
        for (auto &&target : container::reverse(asdlDelete.targets)) {
          evaluate_del(target);
        }
      }
      void Evaluator::evaluate(const library::asdl::Assign &assign) {
        push([&assign](Evaluator *evaluator) {
          evaluator->push(AssignEvaluator{evaluator->stack.top(),
                                          assign.targets.rbegin(),
                                          assign.targets.rend()});
        });
        evaluate_get(assign.value);
      }
      void
      Evaluator::evaluate(const library::asdl::AugAssign & /*aug_assign*/) {}
      void
      Evaluator::evaluate(const library::asdl::AnnAssign & /*ann_assign*/) {}
      void Evaluator::evaluate(const library::asdl::For &asdlFor) {
        push([&asdlFor](Evaluator *evaluatorA) {
          try {
            Evaluator evaluatorB{evaluatorA->thread_context};
            evaluatorB.enter_scope(evaluatorA->self());
            evaluatorB.push([](Evaluator *evaluatorC) {
              evaluatorC->push(CallEvaluator{evaluatorC->stack.top(), {}, {}});
              evaluatorC->stack.pop();
            });
            evaluatorB.get_attribute(evaluatorA->stack.top(), "__next__");
            evaluatorB.evaluate();
          } catch (const object::BaseException &error) {
            if (error.exception.get_attribute("__class__").id() ==
                evaluatorA->builtins().get_attribute("StopIteration").id()) {
              evaluatorA->exit();
              evaluatorA->extend(asdlFor.orelse);
            } else {
              std::cerr << error.what() << std::endl;
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
          evaluatorA->push(CallEvaluator{evaluatorA->stack.top(), {}, {}});
          evaluatorA->stack.pop();
        });
        push([](Evaluator *evaluatorA) {
          evaluatorA->get_attribute(evaluatorA->stack.top(), "__iter__");
          evaluatorA->stack.pop();
        });
        evaluate_get(asdlFor.iter);
        push([](Evaluator *evaluatorA) { evaluatorA->enter(); });
      }
      void Evaluator::evaluate(const library::asdl::AsyncFor & /*async_for*/) {}
      void Evaluator::evaluate(const library::asdl::While &asdlWhile) {
        push([&asdlWhile](Evaluator *evaluatorA) {
          if (evaluatorA->stack.top().get_bool()) {
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
          evaluatorA->stack.pop();
        });
        push([](Evaluator *evaluatorA) {
          evaluatorA->push(ToBoolEvaluator{evaluatorA->stack.top()});
          evaluatorA->stack.pop();
        });
        evaluate_get(asdlWhile.test);
        push([](Evaluator *evaluatorA) { evaluatorA->enter(); });
      }
      void Evaluator::evaluate(const library::asdl::If &asdlIf) {
        push([&asdlIf](Evaluator *evaluator) {
          if (evaluator->stack.top().get_bool()) {
            evaluator->extend(asdlIf.body);
          } else {
            evaluator->extend(asdlIf.orelse);
          }
          evaluator->stack.pop();
        });
        push([](Evaluator *evaluator) {
          evaluator->push(ToBoolEvaluator{evaluator->stack.top()});
          evaluator->stack.pop();
        });
        evaluate_get(asdlIf.test);
      }
      void Evaluator::evaluate(const library::asdl::With &with) {
        push([&with](Evaluator *evaluator) {
          if (auto exception = evaluator->do_try(with.body, {}); exception) {
          } else {
          }
        });
        std::for_each(with.items.rbegin(), with.items.rend(),
                      [this](auto &&withItem) {
                        this->evaluate_get(withItem.context_expr);
                      });
      }
      void
      Evaluator::evaluate(const library::asdl::AsyncWith & /*async_with*/) {}
      void Evaluator::evaluate(const library::asdl::Import &import) {
        std::for_each(
            import.names.rbegin(), import.names.rend(), [this](auto &&alias) {
              if (alias.asname) {
                this->push([&alias](Evaluator *evaluator) {
                  evaluator->self().set_attribute(
                      alias.asname->value, std::move(evaluator->stack.top()));
                  evaluator->stack.pop();
                });
              } else {
                this->push([&alias](Evaluator *evaluator) {
                  evaluator->self().set_attribute(
                      alias.name.value, std::move(evaluator->stack.top()));
                  evaluator->stack.pop();
                });
              }
              this->push([&alias](Evaluator *evaluator) {
                evaluator->push(PushStack{
                    evaluator->thread_context.process_context.import_object(
                        "module_name", alias.name.value)});
              });
            });
      }
      void Evaluator::evaluate(const library::asdl::ImportFrom &importFrom) {
        push([](Evaluator *evaluator) { evaluator->stack.pop(); });
        std::for_each(
            importFrom.names.rbegin(), importFrom.names.rend(),
            [this](auto &&alias) {
              if (alias.asname) {
                this->push([&alias](Evaluator *evaluator) {
                  evaluator->self().set_attribute(
                      alias.asname->value,
                      evaluator->stack.top().get_attribute(alias.name.value));
                });
              } else {
                this->push([&alias](Evaluator *evaluator) {
                  evaluator->self().set_attribute(
                      alias.name.value,
                      evaluator->stack.top().get_attribute(alias.name.value));
                });
              }
            });
        push([&importFrom](Evaluator *evaluator) {
          evaluator->push(
              PushStack{evaluator->thread_context.process_context.import_object(
                  "module_name", importFrom.module.value)});
        });
      }
      void Evaluator::evaluate(const library::asdl::Global & /*global*/) {}
      void Evaluator::evaluate(const library::asdl::Nonlocal & /*nonlocal*/) {}
      void Evaluator::evaluate(const library::asdl::Expr &expr) {
        push([](Evaluator *evaluator) { evaluator->stack.pop(); });
        evaluate_get(expr.value);
      }
      void Evaluator::evaluate(const library::asdl::Raise &raise) {
        if (raise.exc) {
          push([](Evaluator *evaluator) {
            throw object::BaseException{evaluator->stack.top()};
          });
        } else {
          push([](Evaluator * /*evaluator*/) { throw ReRaise{}; });
        }
        if (raise.cause) {
          evaluate_get(*raise.cause);
        }
        if (raise.exc) {
          evaluate_get(*raise.exc);
        }
      }
      void Evaluator::evaluate(const library::asdl::Try &asdlTry) {
        push([](Evaluator *evaluator) {
          if (evaluator->thread_context.ret) {
            evaluator->exit_scope();
          }
        });
        if (auto exception = do_try(asdlTry.body, {}); exception) {
          for (auto &&handler : container::reverse(asdlTry.handlers)) {
            std::visit([](auto &&) {}, handler);
          }
          if (exception = do_try(asdlTry.finalbody, exception); exception) {
            throw object::BaseException{*exception};
          }
          if (exception) {
            throw object::BaseException{*exception};
          }
          return;
        }
        if (auto exception = do_try(asdlTry.orelse, {}); exception) {
          if (exception = do_try(asdlTry.finalbody, exception); exception) {
            throw object::BaseException{*exception};
          }
          if (exception) {
            throw object::BaseException{*exception};
          }
          return;
        }
        extend(asdlTry.finalbody);
      }
      void Evaluator::evaluate(const library::asdl::Assert &assert) {
        if (builtins().get_attribute("__debug__").get_bool()) {
          push([&assert](Evaluator *evaluatorA) {
            if (!evaluatorA->stack.top().get_bool()) {
              return evaluatorA->stack.pop();
            }
            evaluatorA->stack.pop();
            if (!assert.msg) {
              throw object::BaseException{
                  evaluatorA->builtins().get_attribute("AssertionError")};
            }
            evaluatorA->push([](Evaluator *evaluatorB) {
              throw object::BaseException{
                  evaluatorB->builtins().get_attribute("AssertionError")};
            });
            evaluatorA->evaluate_get(*assert.msg);
          });
          evaluate_get(assert.test);
        }
      }
      void Evaluator::evaluate(const library::asdl::Return &asdlReturn) {
        if (asdlReturn.value) {
          push([](Evaluator *evaluator) {
            evaluator->thread_context.ret = std::move(evaluator->stack.top());
            evaluator->stack.pop();
            evaluator->exit_scope();
          });
          evaluate_get(*asdlReturn.value);
        } else {
          exit_scope();
        }
      }
      void Evaluator::evaluate(const library::asdl::Break & /*break*/) {
        push([](Evaluator *evaluator) {
          evaluator->exit();
          evaluator->exit();
        });
      }
      void Evaluator::evaluate(const library::asdl::Continue & /*continue*/) {
        push([](Evaluator *evaluator) { evaluator->exit(); });
      }

      std::optional<object::BaseException>
      Evaluator::do_try(const std::vector<library::asdl::StmtImpl> &body,
                        const std::optional<object::BaseException> &context) {
        if (body.empty()) {
          return context;
        }
        try {
          Evaluator evaluator{thread_context};
          evaluator.enter_scope(self());
          evaluator.extend(body);
          evaluator.evaluate();
        } catch (const object::BaseException &error) {
          std::cerr << error.what() << std::endl;
          object::Object exception = error.exception;
          if (context) {
            exception.set_attribute("__context__", context->exception);
          }
          return object::BaseException{exception};
        } catch (const ReRaise &) {
          if (context) {
            return context;
          }
          return object::BaseException{
              builtins().get_attribute("RuntimeError")};
        } catch (const std::exception &exc) {
          std::cerr << exc.what() << std::endl;
          object::Object exception(object::String{exc.what()}, {});
          if (context) {
            exception.set_attribute("__context__", context->exception);
          }
          return object::BaseException{exception};
        }
        return {};
      }

      void Evaluator::get_attribute(const object::Object &object,
                                    const object::Object &getAttribute,
                                    const std::string &name) {
        if (std::holds_alternative<object::ObjectMethod>(
                getAttribute.value())) {
          if (std::get<object::ObjectMethod>(getAttribute.value()) ==
              object::ObjectMethod::GETATTRIBUTE) {
            if (getAttribute.get_attribute("__class__").id() ==
                thread_context.process_context.global_context.method_id) {
              if (object.has_attribute(name)) {
                return push(PushStack{object.get_attribute(name)});
              }
              auto mro =
                  object.get_attribute("__class__").get_attribute("__mro__");
              for (auto &&type : std::get<object::Tuple>(mro.value())) {
                if (type.has_attribute(name)) {
                  return push(PushStack{type.get_attribute(name)});
                }
              }
              return get_attr(object, name);
            }
          }
        }
        push(CallEvaluator{
            getAttribute,
            {object::Object{object::String(name),
                            {{"__class__", builtins().get_attribute("str")}}}},
            {}});
      }

      void Evaluator::get_attr(const object::Object &object,
                               const std::string &name) {
        push([name](Evaluator *evaluator) {
          evaluator->push(CallEvaluator{
              evaluator->stack.top(),
              {object::Object{
                  object::String(name),
                  {{"__class__", evaluator->builtins().get_attribute("str")}}}},
              {}});
          evaluator->stack.pop();
        });
        if (object.has_attribute("__getattr__")) {
          return push(PushStack{object.get_attribute("__getattr__")});
        }
        auto mro = object.get_attribute("__class__").get_attribute("__mro__");
        for (auto &&type : std::get<object::Tuple>(mro.value())) {
          if (type.has_attribute("__getattr__")) {
            return push(PushStack{type.get_attribute("__getattr__")});
          }
        }
        throw object::BaseException{object::Object{
            {},
            {{"__class__", builtins().get_attribute("AttributeError")},
             {"__class__", builtins().get_attribute("AttributeError")}}}};
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
