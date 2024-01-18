//! handles read expressions and pushes their value to the top of the stack

#pragma once

#include "asdl/asdl.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  struct GetEvaluator {
    explicit GetEvaluator(Evaluator *evaluator) noexcept;
    void evaluate(const asdl::Attribute &attribute) const;
    void evaluate(const asdl::Await &await) const;
    void evaluate(const asdl::Bin &bin) const;
    void evaluate(const asdl::Bool &asdlBool) const;
    void evaluate(const asdl::Call &call) const;
    void evaluate(const asdl::Compare &compare) const;
    void evaluate(const asdl::Dict &dict) const;
    void evaluate(const asdl::DictComp &dictComp) const;
    void evaluate(const asdl::Ellipsis &ellipsis) const;
    void evaluate(const asdl::FormattedValue &formattedValue) const;
    void evaluate(const asdl::GeneratorExp &generatorExp) const;
    void evaluate(const asdl::IfExp &ifExp) const;
    void evaluate(const asdl::JoinedStr &joinedStr) const;
    void evaluate(const asdl::Lambda &lambda) const;
    void evaluate(const asdl::List &list) const;
    void evaluate(const asdl::ListComp &listComp) const;
    void evaluate(const asdl::Name &name) const;
    void evaluate(const asdl::NameConstant &nameConstant) const;
    void evaluate(const asdl::Set &set) const;
    void evaluate(const asdl::SetComp &setComp) const;
    void evaluate(const asdl::Starred &starred) const;
    void evaluate(const asdl::Subscript &subscript) const;
    void evaluate(const asdl::Tuple &tuple) const;
    void evaluate(const asdl::Unary &unary) const;
    void evaluate(const asdl::UnpackDict &unpackDict) const;
    void evaluate(const asdl::Yield &yield) const;
    void evaluate(const asdl::YieldFrom &yieldFrom) const;
    void evaluate(const object::Object &object) const;

  private:
    Evaluator *evaluator;
  };
} // namespace chimera::library::virtual_machine
