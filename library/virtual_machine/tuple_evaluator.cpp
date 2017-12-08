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

//! consumes objects of the stack to form a tuple

#include "virtual_machine/tuple_evaluator.hpp"

#include "object/object.hpp"
#include "virtual_machine/evaluator.hpp"

namespace chimera {
  namespace library {
    namespace virtual_machine {
      void TupleEvaluator::operator()(Evaluator *evaluator) const {
        object::Tuple tuple(evaluator->stack.size() - size);
        for (std::size_t i = 0; evaluator->stack.size() > size; ++i) {
          tuple[tuple.size() - 1 - i] = evaluator->stack.top();
          evaluator->stack.pop();
        }
        evaluator->push(PushStack{object::Object{
            tuple,
            {{"__class__", evaluator->builtins().get_attribute("tuple")}}}});
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
