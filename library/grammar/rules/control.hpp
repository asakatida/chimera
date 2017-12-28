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

//! tao::pegtl::normal that finds embeded rule transformations.

#pragma once

#include <type_traits>

#include <tao/pegtl.hpp>

namespace chimera {
  namespace library {
    namespace grammar {
      namespace rules {
        template <typename Rule, typename = std::void_t<>>
        struct Normal : tao::pegtl::normal<Rule> {};
        template <typename Rule>
        struct Normal<Rule, std::void_t<typename Rule::Transform>>
            : tao::pegtl::normal<Rule> {
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename ProcessContext, typename Outer>
          static bool match(Input &in, ProcessContext &&processContext,
                            Outer &&outer) {
            typename Rule::Transform state;

            if (tao::pegtl::normal<Rule>::template match<A, M, Action, Control>(
                    in, processContext, state)) {
              state.success(outer);
              return true;
            }
            return false;
          }
        };
      } // namespace rules
    }   // namespace grammar
  }     // namespace library
} // namespace chimera
