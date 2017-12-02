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

//! handles individual targets of assignment expressions like
//! a = b = c = ...
//!     ^

#include "virtual_machine/set_evaluator.hpp"

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      void SetEvaluator::evaluate(const library::asdl::Bool & /*asdlBool*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Bin & /*bin*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Unary & /*unary*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Lambda & /*lambda*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::IfExp & /*ifExp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const library::asdl::ListComp & /*list_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::SetComp & /*set_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const library::asdl::DictComp & /*dict_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(
          const library::asdl::GeneratorExp & /*generator_exp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Await & /*await*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Yield & /*yield*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const library::asdl::YieldFrom & /*yield_from*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Compare & /*compare*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Call & /*call*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Attribute &attribute) {
        evaluator->push([&attribute](Evaluator *evaluatorA) {
          auto value = std::move(evaluatorA->stack.top());
          evaluatorA->stack.pop();
          evaluatorA->stack.top().set_attribute(attribute.attr.value,
                                                std::move(value));
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(attribute.value);
      }
      void SetEvaluator::evaluate(const library::asdl::Subscript &subscript) {
        evaluator->evaluate_get(subscript.value);
      }
      void SetEvaluator::evaluate(const library::asdl::Starred & /*starred*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(
          const library::asdl::NameConstant & /*nameConstant*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const library::asdl::Ellipsis & /*ellipsis*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(
          const library::asdl::FormattedValue & /*formatted_value*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const library::asdl::JoinedStr & /*joined_str*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Name &name) {
        evaluator->push([&name](Evaluator *evaluatorA) {
          evaluatorA->self().set_attribute(name.value, evaluatorA->stack.top());
        });
      }
      void SetEvaluator::evaluate(const library::asdl::Dict & /*dict*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::Set & /*set*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const library::asdl::List & /*list*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void SetEvaluator::evaluate(const library::asdl::Tuple & /*tuple*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void
      SetEvaluator::evaluate(const library::asdl::Constant & /*constant*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
