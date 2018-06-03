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

//! handles read expressions and pushes their value to the top of the stack

#include "virtual_machine/get_evaluator.hpp"

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      void GetEvaluator::evaluate(const asdl::Bool &asdlBool) {
        auto begin = asdlBool.values.begin();
        ++begin;
        switch (asdlBool.op) {
          case asdl::Bool::AND:
            evaluator->push(BoolAndEvaluator{begin, asdlBool.values.end()});
            break;
          case asdl::Bool::OR:
            evaluator->push(BoolOrEvaluator{begin, asdlBool.values.end()});
            break;
        }
        evaluator->push([](Evaluator *evaluatorA) {
          evaluatorA->push(ToBoolEvaluator{evaluatorA->stack.top()});
        });
        evaluator->evaluate_get(asdlBool.values.front());
      }
      void GetEvaluator::evaluate(const asdl::Bin &bin) {
        auto begin = bin.values.begin();
        ++begin;
        switch (bin.op) {
          case asdl::Operator::ADD:
            evaluator->push(BinAddEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::SUB:
            evaluator->push(BinSubEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::MULT:
            evaluator->push(BinMultEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::MAT_MULT:
            evaluator->push(BinMatMultEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::DIV:
            evaluator->push(BinDivEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::MOD:
            evaluator->push(BinModEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::POW:
            evaluator->push(BinPowEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::L_SHIFT:
            evaluator->push(BinLShiftEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::R_SHIFT:
            evaluator->push(BinRShiftEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::BIT_OR:
            evaluator->push(BinBitOrEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::BIT_XOR:
            evaluator->push(BinBitXorEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::BIT_AND:
            evaluator->push(BinBitAndEvaluator{begin, bin.values.end()});
            break;
          case asdl::Operator::FLOOR_DIV:
            evaluator->push(BinFloorDivEvaluator{begin, bin.values.end()});
            break;
        }
        evaluator->evaluate_get(bin.values.front());
      }
      void GetEvaluator::evaluate(const asdl::Unary &unary) {
        switch (unary.op) {
          case asdl::Unary::BIT_NOT:
            evaluator->push(UnaryBitNotEvaluator{});
            break;
          case asdl::Unary::NOT:
            evaluator->push(UnaryNotEvaluator{});
            break;
          case asdl::Unary::ADD:
            evaluator->push(UnaryAddEvaluator{});
            break;
          case asdl::Unary::SUB:
            evaluator->push(UnarySubEvaluator{});
            break;
        }
        evaluator->evaluate_get(unary.operand);
      }
      void GetEvaluator::evaluate(const asdl::Lambda & /*lambda*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::IfExp &ifExp) {
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
      void GetEvaluator::evaluate(const asdl::ListComp & /*list_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::SetComp & /*set_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::UnpackDict & /*unpackDict*/) {}
      void GetEvaluator::evaluate(const asdl::DictComp & /*dict_comp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void
      GetEvaluator::evaluate(const asdl::GeneratorExp & /*generator_exp*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::Await & /*await*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::Yield & /*yield*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::YieldFrom & /*yield_from*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::Compare & /*compare*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::Call &call) {
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
      void GetEvaluator::evaluate(const asdl::Attribute &attribute) {
        evaluator->push([&attribute](Evaluator *evaluatorA) {
          evaluatorA->get_attribute(evaluatorA->stack.top(),
                                    attribute.attr.value);
          evaluatorA->stack.pop();
        });
        evaluator->evaluate_get(attribute.value);
      }
      void GetEvaluator::evaluate(const asdl::Subscript &subscript) {
        evaluator->evaluate_get(subscript.value);
      }
      void GetEvaluator::evaluate(const asdl::Starred &starred) {
        evaluator->evaluate_get(starred.value);
      }
      void GetEvaluator::evaluate(const asdl::NameConstant &nameConstant) {
        switch (nameConstant.value) {
          case asdl::NameConstant::FALSE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("False")});
            break;
          case asdl::NameConstant::NONE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("None")});
            break;
          case asdl::NameConstant::TRUE:
            evaluator->push(
                PushStack{evaluator->builtins().get_attribute("True")});
            break;
        }
      }
      void GetEvaluator::evaluate(const asdl::Ellipsis & /*ellipsis*/) {
        evaluator->push(
            PushStack{evaluator->builtins().get_attribute("Ellipsis")});
      }
      void GetEvaluator::evaluate(const asdl::FormattedValue &formattedValue) {
        evaluator->evaluate_get(formattedValue.value);
      }
      void GetEvaluator::evaluate(const asdl::JoinedStr & /*joined_str*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
      }
      void GetEvaluator::evaluate(const asdl::Name &name) {
        evaluator->get_attribute(evaluator->self(), name.value);
      }
      void GetEvaluator::evaluate(const asdl::Dict & /*dict*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("dict")});
      }
      void GetEvaluator::evaluate(const asdl::Set & /*set*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("set")});
      }
      void GetEvaluator::evaluate(const asdl::List & /*list*/) {
        evaluator->push(PushStack{evaluator->builtins().get_attribute("list")});
      }
      void GetEvaluator::evaluate(const asdl::Tuple &tuple) {
        evaluator->push(TupleEvaluator{evaluator->stack.size()});
        evaluator->extend(tuple.elts);
      }
      void GetEvaluator::evaluate(const asdl::Constant &constant) {
        evaluator->push(PushStack{evaluator->thread_context.process_context
                                      .constants[constant.constant]});
      }
      void GetEvaluator::evaluate(const object::Object &object) {
        evaluator->push(PushStack{object});
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
