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

//! expose ::rules::* for easy use.

#pragma once

#include <tao/pegtl.hpp>

#include "grammar/rules/rules.hpp"

namespace chimera::library::grammar {
  using tao::pegtl::at;
  using tao::pegtl::discard;
  using tao::pegtl::eof;
  using tao::pegtl::failure;
  using tao::pegtl::if_must;
  using tao::pegtl::if_then_else;
  using tao::pegtl::list;
  using tao::pegtl::list_must;
  using tao::pegtl::list_tail;
  using tao::pegtl::minus;
  using tao::pegtl::must;
  using tao::pegtl::not_at;
  using tao::pegtl::opt;
  using tao::pegtl::opt_must;
  using tao::pegtl::plus;
  using tao::pegtl::rep;
  using tao::pegtl::rep_opt;
  using tao::pegtl::seq;
  using tao::pegtl::sor;
  using tao::pegtl::star;
  using tao::pegtl::success;
  using tao::pegtl::until;
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  using tao::pegtl::any;
  using tao::pegtl::not_one;
  using tao::pegtl::not_range;
  using tao::pegtl::one;
  using tao::pegtl::range;
  using tao::pegtl::ranges;
#else
  using tao::pegtl::utf8::any;
  using tao::pegtl::utf8::not_one;
  using tao::pegtl::utf8::not_range;
  using tao::pegtl::utf8::one;
  using tao::pegtl::utf8::range;
  using tao::pegtl::utf8::ranges;
#endif
} // namespace chimera::library::grammar
