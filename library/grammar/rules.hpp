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

//! tao::pegtl::* wrappers to control stack specialization

#pragma once

#include <metal.hpp>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/rep_one_min_max.hpp>

#include "grammar/rules/rules.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      using rules::Action;
      using rules::Any;
      using rules::At;
      using rules::Control;
      using rules::Digit;
      using rules::Discard;
      using rules::Ellipsis;
      using rules::Eof;
      using rules::Eol;
      using rules::Eolf;
      using rules::Failure;
      using rules::IfMust;
      using rules::List;
      using rules::ListMust;
      using rules::ListTail;
      using rules::Minus;
      using rules::Must;
      using rules::Normal;
      using rules::NotAt;
      using rules::NotOne;
      using rules::Nothing;
      using rules::Nul;
      using rules::One;
      using rules::One;
      using rules::Opt;
      using rules::Plus;
      using rules::Range;
      using rules::Ranges;
      using rules::Rep;
      using rules::RepOpt;
      using rules::Seq;
      using rules::Seven;
      using rules::Sor;
      using rules::Star;
      using rules::String;
      using rules::Success;
      using rules::Until;
      using rules::Xdigit;
      using tao::pegtl::if_then_else;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
