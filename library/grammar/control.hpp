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

//! helpers needed by all parse rules.

#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/changes.hpp>
#include <tao/pegtl/contrib/tracer.hpp>

#include "asdl/asdl.hpp"
#include "grammar/stack.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename Rule>
      struct Actions : tao::pegtl::nothing<Rule> {};
      template <typename Rule>
      struct ControlBase : tao::pegtl::normal<Rule> {};
      template <typename Rule>
      struct Control : ControlBase<Rule> {};
      template <typename Rule, typename State>
      using ChangeState = tao::pegtl::change_state<Rule, State, ControlBase>;
      template <typename... Rules>
      using InputRule =
          tao::pegtl::control<Control, tao::pegtl::action<Actions, Rules...>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
