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

#include <gsl/gsl>
#include <tao/pegtl.hpp>

#include <type_traits>

namespace chimera::library::grammar {
  template <template <typename...> class OtherControl = tao::pegtl::normal>
  struct MakeControl {
    template <typename Rule, typename = std::void_t<>>
    struct Normal : OtherControl<Rule> {};
    template <typename Rule>
    struct Normal<Rule, std::void_t<decltype(typename Rule::Transform{})>>
        : OtherControl<Rule> {
      using LocalControl = OtherControl<Rule>;
      template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                template <typename...> class Action,
                template <typename...> class Control, typename Input,
                typename Outer, typename... Args>
      static auto match(Input &&input, Outer &&outer, Args &&...args) -> bool {
        if constexpr (A == tao::pegtl::apply_mode::action) {
          auto t = outer.transaction();
          typename Rule::Transform state;
          if (LocalControl::template match<A, M, Action, Control>(
                  input, state, std::forward<Args>(args)...)) {
            state.finalize(state, std::forward<Outer>(outer));
            t.commit();
            return true;
          }
          return false;
        }
        if constexpr (A != tao::pegtl::apply_mode::action) {
          return LocalControl::template match<A, M, Action, Control>(
              input, std::forward<Outer>(outer), std::forward<Args>(args)...);
        }
        Expects(false);
      }
    };
  };
} // namespace chimera::library::grammar
