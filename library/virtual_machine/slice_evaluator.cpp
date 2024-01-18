//! constructs a slice object from a slice expression

#include "virtual_machine/slice_evaluator.hpp"

#include "virtual_machine/evaluator.hpp"

namespace chimera::library::virtual_machine {
  SliceEvaluator::SliceEvaluator(Evaluator *evaluator) noexcept
      : evaluator(evaluator) {}
  void SliceEvaluator::operator()(const asdl::Slice & /*slice*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void SliceEvaluator::operator()(const asdl::ExtSlice & /*ext_slice*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
  void SliceEvaluator::operator()(const asdl::Index & /*index*/) const {
    evaluator->push(PushStack{evaluator->builtins().get_attribute("None")});
  }
} // namespace chimera::library::virtual_machine
