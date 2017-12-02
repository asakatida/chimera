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

//! handles read expressions and pushes their value to the top of the stack

#include "virtual_machine/get_evaluator.hpp"

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      void GetEvaluator::evaluate(const library::asdl::Bool &asdlBool) {
        auto begin = asdlBool.values.begin();
        ++begin;
        switch (asdlBool.op) {
          case library::asdl::Bool::AND:
            evaluator->push(BoolAndEvaluator{begin, asdlBool.values.end()});
            break;
          case library::asdl::Bool::OR:
            evaluator->push(BoolOrEvaluator{begin, asdlBool.values.end()});
            break;
        }
        evaluator->push([](Evaluator *evaluatorA) {
          evaluatorA->push(ToBoolEvaluator{evaluatorA->stack.top()});
        });
        evaluator->evaluate_get(asdlBool.values.front());
      }
      void GetEvaluator::evaluate(const library::asdl::Bin &bin) {
        auto begin = bin.values.begin();
        ++begin;
        switch (bin.op) {
          case library::asdl::Operator::ADD:
            evaluator->push(BinAddEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::SUB:
            evaluator->push(BinSubEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::MULT:
            evaluator->push(BinMultEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::MAT_MULT:
            evaluator->push(BinMatMultEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::DIV:
            evaluator->push(BinDivEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::MOD:
            evaluator->push(BinModEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::POW:
            evaluator->push(BinPowEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::L_SHIFT:
            evaluator->push(BinLShiftEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::R_SHIFT:
            evaluator->push(BinRShiftEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::BIT_OR:
            evaluator->push(BinBitOrEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::BIT_XOR:
            evaluator->push(BinBitXorEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::BIT_AND:
            evaluator->push(BinBitAndEvaluator{begin, bin.values.end()});
            break;
          case library::asdl::Operator::FLOOR_DIV:
            evaluator->push(BinFloorDivEvaluator{begin, bin.values.end()});
            break;
        }
        evaluator->evaluate_get(bin.values.front());
      }
      void GetEvaluator::evaluate(const library::asdl::Unary &unary) {
        switch (unary.op) {
          case library::asdl::Unary::BIT_NOT:
            evaluator->push(UnaryBitNotEvaluator{});
            break;
          case library::asdl::Unary::NOT:
            evaluator->push(UnaryNotEvaluator{});
            break;
          case library::asdl::Unary::ADD:
            evaluator->push(UnaryAddEvaluator{});
            break;
          case library::asdl::Unary::SUB:
            evaluator->push(UnarySubEvaluator{});
            break;
        }
        evaluator->evaluate_get(unary.operand);
      }
      void GetEvaluator::evaluate(const library::asdl::Lambda & /*lambda*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::IfExp &ifExp) {
        evaluator->push([&ifExp](Evaluator *evaluatorA) {
          evaluatorA->evaluate_get(
              evaluatorA->stack.top().get_bool() ? ifExp.body : ifExp.orelse);
          evaluatorA->stack.pop();
        });
        evaluator->push([](Evaluator *evaluatorA) {
          evaluatorA->push(ToBoolEvaluator{evaluatorA->stack.top()});
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(ifExp.test);
      }
      void
      GetEvaluator::evaluate(const library::asdl::ListComp & /*list_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::SetComp & /*set_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void
      GetEvaluator::evaluate(const library::asdl::DictComp & /*dict_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(
          const library::asdl::GeneratorExp & /*generator_exp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::Await & /*await*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::Yield & /*yield*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void
      GetEvaluator::evaluate(const library::asdl::YieldFrom & /*yield_from*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::Compare & /*compare*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::Call &call) {
        evaluator->push([](Evaluator *evaluatorA) {
          evaluatorA->push(PushStack{
              std::move(evaluatorA->thread_context.ret)
                  .value_or(evaluatorA->builtins().get_attribute("None"))});
        });
        evaluator->extend(call.args);
        evaluator->push([](Evaluator *evaluatorA) {
          auto top = evaluatorA->stack.top().copy({});
          evaluatorA->stack.pop();
          evaluatorA->enter_scope(top);
        });
        evaluator->evaluate_get(call.func);
      }
      void GetEvaluator::evaluate(const library::asdl::Attribute &attribute) {
        evaluator->push([&attribute](Evaluator *evaluatorA) {
          evaluatorA->get_attribute(evaluatorA->stack.top(),
                                    attribute.attr.value);
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(attribute.value);
      }
      void GetEvaluator::evaluate(const library::asdl::Subscript &subscript) {
        evaluator->evaluate_get(subscript.value);
      }
      void GetEvaluator::evaluate(const library::asdl::Starred &starred) {
        evaluator->evaluate_get(starred.value);
      }
      void
      GetEvaluator::evaluate(const library::asdl::NameConstant &nameConstant) {
        switch (nameConstant.value) {
          case library::asdl::NameConstant::FALSE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("False")});
            break;
          case library::asdl::NameConstant::NONE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("None")});
            break;
          case library::asdl::NameConstant::TRUE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("True")});
            break;
        }
      }
      void
      GetEvaluator::evaluate(const library::asdl::Ellipsis & /*ellipsis*/) {
        evaluator->push(
            PushStack{evaluator->builtins().get_attribute("Ellipsis")});
      }
      void GetEvaluator::evaluate(
          const library::asdl::FormattedValue &formattedValue) {
        evaluator->evaluate_get(formattedValue.value);
      }
      void
      GetEvaluator::evaluate(const library::asdl::JoinedStr & /*joined_str*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const library::asdl::Name &name) {
        evaluator->get_attribute(evaluator->self(), name.value);
      }
      void GetEvaluator::evaluate(const library::asdl::Dict & /*dict*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("dict")});
      }
      void GetEvaluator::evaluate(const library::asdl::Set & /*set*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("set")});
      }
      void GetEvaluator::evaluate(const library::asdl::List & /*list*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("list")});
      }
      void GetEvaluator::evaluate(const library::asdl::Tuple &tuple) {
        evaluator->push(TupleEvaluator{evaluator->stack.size()});
        evaluator->extend(tuple.elts);
      }
      void GetEvaluator::evaluate(const library::asdl::Constant &constant) {
        evaluator->push(
            PushStack{evaluator->thread_context.constants[constant.constant]});
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
