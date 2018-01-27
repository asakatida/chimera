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

//! tao::pegtl::* wrappers to control stack specialization

#pragma once

#include <metal.hpp>
#include <tao/pegtl.hpp>

#include "grammar/rules/action.hpp"
#include "grammar/rules/captures.hpp"
#include "grammar/rules/control.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace rules {
        template <typename... Types>
        struct Tuple {};

        template <typename... Types>
        struct Variant {};

        template <typename Type>
        struct Vector {};

        using tao::pegtl::opt;
        using tao::pegtl::rep;
        using tao::pegtl::seq;
        using tao::pegtl::sor;

        template <typename Rule>
        struct ASTType {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<opt<Rules...>> {
          using Type = Variant<Tuple<>, Tuple<>>;
        };
        template <unsigned Repeat, typename... Rules>
        struct ASTType<rep<Repeat, Rules...>> {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<seq<Rules...>> {
          using Type = Tuple<>;
        };
        template <typename... Rules>
        struct ASTType<sor<Rules...>> {
          using Type = Variant<>;
        };
      } // namespace rules
    }   // namespace grammar
  }     // namespace library
} // namespace chimera
