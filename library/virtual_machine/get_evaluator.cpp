//! handles read expressions and pushes their value to the top of the stack

#include "virtual_machine/get_evaluator.hpp"

#include "asdl/asdl.hpp"
#include "object/object.hpp"
#include "virtual_machine/bin_evaluator.hpp"
#include "virtual_machine/bool_evaluator.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/push_stack.hpp"
#include "virtual_machine/to_bool_evaluator.hpp"
#include "virtual_machine/tuple_evaluator.hpp"
#include "virtual_machine/unary_evaluator.hpp"

namespace chimera::library::virtual_machine {
  GetEvaluator::GetEvaluator(Evaluator *evaluator) noexcept
      : evaluator(evaluator) {}
  void GetEvaluator::evaluate(const asdl::Bool &asdlBool) const {
    switch (asdlBool.op) {
      case asdl::Bool::AND:
        evaluator->push(BoolAndEvaluator{asdlBool.values});
        break;
      case asdl::Bool::OR:
        evaluator->push(BoolOrEvaluator{asdlBool.values});
        break;
    }
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(ToBoolEvaluator{evaluatorA->stack_top()});
    });
    evaluator->evaluate_get(asdlBool.values.front());
  }
  void GetEvaluator::evaluate(const asdl::Bin &bin) const {
    switch (bin.op) {
      case asdl::Operator::ADD:
        evaluator->push(BinAddEvaluator{bin.values});
        break;
      case asdl::Operator::SUB:
        evaluator->push(BinSubEvaluator{bin.values});
        break;
      case asdl::Operator::MULT:
        evaluator->push(BinMultEvaluator{bin.values});
        break;
      case asdl::Operator::MAT_MULT:
        evaluator->push(BinMatMultEvaluator{bin.values});
        break;
      case asdl::Operator::DIV:
        evaluator->push(BinDivEvaluator{bin.values});
        break;
      case asdl::Operator::MOD:
        evaluator->push(BinModEvaluator{bin.values});
        break;
      case asdl::Operator::POW:
        evaluator->push(BinPowEvaluator{bin.values});
        break;
      case asdl::Operator::L_SHIFT:
        evaluator->push(BinLShiftEvaluator{bin.values});
        break;
      case asdl::Operator::R_SHIFT:
        evaluator->push(BinRShiftEvaluator{bin.values});
        break;
      case asdl::Operator::BIT_OR:
        evaluator->push(BinBitOrEvaluator{bin.values});
        break;
      case asdl::Operator::BIT_XOR:
        evaluator->push(BinBitXorEvaluator{bin.values});
        break;
      case asdl::Operator::BIT_AND:
        evaluator->push(BinBitAndEvaluator{bin.values});
        break;
      case asdl::Operator::FLOOR_DIV:
        evaluator->push(BinFloorDivEvaluator{bin.values});
        break;
    }
    evaluator->evaluate_get(bin.values.front());
  }
  void GetEvaluator::evaluate(const asdl::Unary &unary) const {
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
  void GetEvaluator::evaluate(const asdl::Lambda & /*lambda*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::IfExp &ifExp) const {
    evaluator->push([&ifExp](Evaluator *evaluatorA) {
      evaluatorA->evaluate_get(
          evaluatorA->stack_top().get_bool() ? ifExp.body : ifExp.orelse);
      evaluatorA->stack_pop();
    });
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(ToBoolEvaluator{evaluatorA->stack_top()});
      evaluatorA->stack_pop();
    });
    evaluator->evaluate_get(ifExp.test);
  }
  void GetEvaluator::evaluate(const asdl::ListComp & /*list_comp*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::SetComp & /*set_comp*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::UnpackDict & /*unpackDict*/) const {}
  void GetEvaluator::evaluate(const asdl::DictComp & /*dict_comp*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void
  GetEvaluator::evaluate(const asdl::GeneratorExp & /*generator_exp*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::Await & /*await*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::Yield & /*yield*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::YieldFrom & /*yield_from*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::Compare & /*compare*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::Call &call) const {
    evaluator->push([](Evaluator *evaluatorA) {
      evaluatorA->push(PushStack{evaluatorA->return_value()});
    });
    evaluator->extend(call.args);
    evaluator->push([](Evaluator *evaluatorA) {
      auto top = evaluatorA->stack_remove();
      evaluatorA->enter_scope(top);
    });
    evaluator->evaluate_get(call.func);
  }
  void GetEvaluator::evaluate(const asdl::Attribute &attribute) const {
    evaluator->push([&attribute](Evaluator *evaluatorA) {
      evaluatorA->get_attribute(evaluatorA->stack_top(), attribute.attr.value);
      evaluatorA->stack_pop();
    });
    evaluator->evaluate_get(attribute.value);
  }
  void GetEvaluator::evaluate(const asdl::Subscript &subscript) const {
    evaluator->evaluate_get(subscript.value);
  }
  void GetEvaluator::evaluate(const asdl::Starred &starred) const {
    evaluator->evaluate_get(starred.value);
  }
  void GetEvaluator::evaluate(const asdl::NameConstant &nameConstant) const {
    switch (nameConstant.value) {
      case asdl::NameConstant::FALSE:
        evaluator->push(
            PushStack{evaluator->builtins().get_attribute("False")});
        break;
      case asdl::NameConstant::NONE:
        evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
        break;
      case asdl::NameConstant::TRUE:
        evaluator->push(PushStack{evaluator->builtins().get_attribute("True")});
        break;
    }
  }
  void GetEvaluator::evaluate(const asdl::Ellipsis & /*ellipsis*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("Ellipsis")});
  }
  void
  GetEvaluator::evaluate(const asdl::FormattedValue &formattedValue) const {
    evaluator->evaluate_get(formattedValue.value);
  }
  void GetEvaluator::evaluate(const asdl::JoinedStr & /*joined_str*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void GetEvaluator::evaluate(const asdl::Name &name) const {
    evaluator->get_attribute(evaluator->self(), name.value);
  }
  void GetEvaluator::evaluate(const asdl::Dict & /*dict*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("dict")});
  }
  void GetEvaluator::evaluate(const asdl::Set & /*set*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("set")});
  }
  void GetEvaluator::evaluate(const asdl::List & /*list*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("list")});
  }
  void GetEvaluator::evaluate(const asdl::Tuple &tuple) const {
    evaluator->push(TupleEvaluator{evaluator->stack_size()});
    evaluator->extend(tuple.elts);
  }
  void GetEvaluator::evaluate(const object::Object &object) const {
    evaluator->push(PushStack{object});
  }
} // namespace chimera::library::virtual_machine
