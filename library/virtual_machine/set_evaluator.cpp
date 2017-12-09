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
      void SetEvaluator::evaluate(const asdl::Bool & /*asdlBool*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Bin & /*bin*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Unary & /*unary*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Lambda & /*lambda*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::IfExp & /*ifExp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::ListComp & /*list_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::SetComp & /*set_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::DictComp & /*dict_comp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const asdl::GeneratorExp & /*generator_exp*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Await & /*await*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Yield & /*yield*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::YieldFrom & /*yield_from*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Compare & /*compare*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Call & /*call*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Attribute &attribute) {
        evaluator->push([&attribute](Evaluator *evaluatorA) {
          auto value = std::move(evaluatorA->stack.top());
          evaluatorA->stack.pop();
          evaluatorA->stack.top().set_attribute(attribute.attr.value,
                                                std::move(value));
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(attribute.value);
      }
      void SetEvaluator::evaluate(const asdl::Subscript &subscript) {
        evaluator->evaluate_get(subscript.value);
      }
      void SetEvaluator::evaluate(const asdl::Starred & /*starred*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::NameConstant & /*nameConstant*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Ellipsis & /*ellipsis*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void
      SetEvaluator::evaluate(const asdl::FormattedValue & /*formatted_value*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::JoinedStr & /*joined_str*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Name &name) {
        evaluator->push([&name](Evaluator *evaluatorA) {
          evaluatorA->self().set_attribute(name.value, evaluatorA->stack.top());
        });
      }
      void SetEvaluator::evaluate(const asdl::Dict & /*dict*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::Set & /*set*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
      void SetEvaluator::evaluate(const asdl::List & /*list*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void SetEvaluator::evaluate(const asdl::Tuple & /*tuple*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void SetEvaluator::evaluate(const asdl::Constant & /*constant*/) {
        throw object::BaseException{
            evaluator->builtins().get_attribute("RuntimeError")};
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
