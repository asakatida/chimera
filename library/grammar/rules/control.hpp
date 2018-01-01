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

//! tao::pegtl::normal that finds embeded rule transformations.
//! based on tao::pegtl::change_state.

#pragma once

#include <type_traits>

#include <tao/pegtl.hpp>

namespace chimera {
  namespace library {
    namespace grammar {
      namespace rules {
        namespace detail {
          template <typename Rule, tao::pegtl::apply_mode A>
          struct Match : tao::pegtl::normal<Rule> {};
          template <typename Rule>
          struct Match<Rule, tao::pegtl::apply_mode::ACTION> {
            template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                      template <typename...> class Action,
                      template <typename...> class Control, typename Input,
                      typename Outer, typename ProcessContext>
            static bool match(Input &in, Outer &&outer,
                              ProcessContext &&processContext) {
              typename Rule::Transform state;

              if (tao::pegtl::normal<Rule>::template match<A, M, Action,
                                                           Control>(
                      in, state, processContext)) {
                state.success(outer);
                return true;
              }
              return false;
            }
          };
        } // namespace detail
        template <typename Rule, typename = std::void_t<>>
        struct Normal : tao::pegtl::normal<Rule> {};
        template <typename Rule>
        struct Normal<Rule, std::void_t<typename Rule::Transform>>
            : tao::pegtl::normal<Rule> {
          struct BlankState {
            template <typename Outer>
            void success(const Outer & /*outer*/) {}
          };
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename... Args>
          static bool match(Input &in, Args &&... args) {
            return detail::Match<Rule, A>::template match<A, M, Action,
                                                          Control>(
                in, std::forward<Args>(args)...);
          }
        };
      } // namespace rules
    }   // namespace grammar
  }     // namespace library
} // namespace chimera
