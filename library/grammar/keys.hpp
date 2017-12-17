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

//! parse definitions for keyword tokens.

#pragma once

#include <tao/pegtl.hpp>

#include "grammar/identifier.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, typename String>
      using Key = Token<Implicit, String, tao::pegtl::not_at<XidContinue>>;
      template <bool Implicit>
      struct And : Key<Implicit, tao::pegtl::string<'a', 'n', 'd'>> {};
      template <bool Implicit>
      struct As : Key<Implicit, tao::pegtl::string<'a', 's'>> {};
      template <bool Implicit>
      struct Assert
          : Key<Implicit, tao::pegtl::string<'a', 's', 's', 'e', 'r', 't'>> {};
      template <bool Implicit>
      struct Async
          : Key<Implicit, tao::pegtl::string<'a', 's', 'y', 'n', 'c'>> {};
      template <bool Implicit>
      struct Await
          : Key<Implicit, tao::pegtl::string<'a', 'w', 'a', 'i', 't'>> {};
      template <bool Implicit>
      struct Break
          : Key<Implicit, tao::pegtl::string<'b', 'r', 'e', 'a', 'k'>> {};
      template <bool Implicit>
      struct Class
          : Key<Implicit, tao::pegtl::string<'c', 'l', 'a', 's', 's'>> {};
      template <bool Implicit>
      struct Continue
          : Key<Implicit,
                tao::pegtl::string<'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>> {};
      template <bool Implicit>
      struct Def : Key<Implicit, tao::pegtl::string<'d', 'e', 'f'>> {};
      template <bool Implicit>
      struct Del : Key<Implicit, tao::pegtl::string<'d', 'e', 'l'>> {};
      template <bool Implicit>
      struct Elif : Key<Implicit, tao::pegtl::string<'e', 'l', 'i', 'f'>> {};
      template <bool Implicit>
      struct Else : Key<Implicit, tao::pegtl::string<'e', 'l', 's', 'e'>> {};
      template <bool Implicit>
      struct Except
          : Key<Implicit, tao::pegtl::string<'e', 'x', 'c', 'e', 'p', 't'>> {};
      template <bool Implicit>
      struct False
          : Key<Implicit, tao::pegtl::string<'F', 'a', 'l', 's', 'e'>> {};
      template <bool Implicit>
      struct Actions<False<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::FALSE}});
        }
      };
      template <bool Implicit>
      struct Finally
          : Key<Implicit,
                tao::pegtl::string<'f', 'i', 'n', 'a', 'l', 'l', 'y'>> {};
      template <bool Implicit>
      struct For : Key<Implicit, tao::pegtl::string<'f', 'o', 'r'>> {};
      template <bool Implicit>
      struct From : Key<Implicit, tao::pegtl::string<'f', 'r', 'o', 'm'>> {};
      template <bool Implicit>
      struct Global
          : Key<Implicit, tao::pegtl::string<'g', 'l', 'o', 'b', 'a', 'l'>> {};
      template <bool Implicit>
      struct If : Key<Implicit, tao::pegtl::string<'i', 'f'>> {};
      template <bool Implicit>
      struct Import
          : Key<Implicit, tao::pegtl::string<'i', 'm', 'p', 'o', 'r', 't'>> {};
      template <bool Implicit>
      struct InImpl : Key<Implicit, tao::pegtl::string<'i', 'n'>> {};
      template <bool Implicit>
      struct In : tao::pegtl::seq<InImpl<Implicit>> {};
      template <bool Implicit>
      struct InOp : tao::pegtl::seq<InImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<InOp<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IN);
        }
      };
      template <bool Implicit>
      struct IsImpl : Key<Implicit, tao::pegtl::string<'i', 's'>> {};
      template <bool Implicit>
      struct Lambda
          : Key<Implicit, tao::pegtl::string<'l', 'a', 'm', 'b', 'd', 'a'>> {};
      template <bool Implicit>
      struct None : Key<Implicit, tao::pegtl::string<'N', 'o', 'n', 'e'>> {};
      template <bool Implicit>
      struct Actions<None<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::NONE}});
        }
      };
      template <bool Implicit>
      struct Nonlocal
          : Key<Implicit,
                tao::pegtl::string<'n', 'o', 'n', 'l', 'o', 'c', 'a', 'l'>> {};
      template <bool Implicit>
      struct NotImpl : Key<Implicit, tao::pegtl::string<'n', 'o', 't'>> {};
      template <bool Implicit>
      struct NotIn : tao::pegtl::seq<NotImpl<Implicit>, InImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<NotIn<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::NOT_IN);
        }
      };
      template <bool Implicit>
      struct Is : tao::pegtl::seq<IsImpl<Implicit>,
                                  tao::pegtl::not_at<NotImpl<false>>> {};
      template <bool Implicit>
      struct Actions<Is<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IS);
        }
      };
      template <bool Implicit>
      struct IsNot : tao::pegtl::seq<IsImpl<Implicit>, NotImpl<Implicit>> {};
      template <bool Implicit>
      struct Actions<IsNot<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IS_NOT);
        }
      };
      template <bool Implicit>
      struct Not : tao::pegtl::seq<NotImpl<Implicit>,
                                   tao::pegtl::not_at<InImpl<false>>> {};
      template <bool Implicit>
      struct Or : Key<Implicit, tao::pegtl::string<'o', 'r'>> {};
      template <bool Implicit>
      struct Pass : Key<Implicit, tao::pegtl::string<'p', 'a', 's', 's'>> {};
      template <bool Implicit>
      struct Raise
          : Key<Implicit, tao::pegtl::string<'r', 'a', 'i', 's', 'e'>> {};
      template <bool Implicit>
      struct Return
          : Key<Implicit, tao::pegtl::string<'r', 'e', 't', 'u', 'r', 'n'>> {};
      template <bool Implicit>
      struct True : Key<Implicit, tao::pegtl::string<'T', 'r', 'u', 'e'>> {};
      template <bool Implicit>
      struct Actions<True<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::TRUE}});
        }
      };
      template <bool Implicit>
      struct Try : Key<Implicit, tao::pegtl::string<'t', 'r', 'y'>> {};
      template <bool Implicit>
      struct While
          : Key<Implicit, tao::pegtl::string<'w', 'h', 'i', 'l', 'e'>> {};
      template <bool Implicit>
      struct With : Key<Implicit, tao::pegtl::string<'w', 'i', 't', 'h'>> {};
      template <bool Implicit>
      struct Yield
          : Key<Implicit, tao::pegtl::string<'y', 'i', 'e', 'l', 'd'>> {};
      using Keywords = tao::pegtl::sor<
          And<false>, As<false>, Assert<false>, Break<false>, Class<false>,
          Continue<false>, Def<false>, Del<false>, Elif<false>, Else<false>,
          Except<false>, False<false>, Finally<false>, For<false>, From<false>,
          Global<false>, If<false>, Import<false>, InImpl<false>, IsImpl<false>,
          Lambda<false>, None<false>, Nonlocal<false>, NotImpl<false>,
          Or<false>, Pass<false>, Raise<false>, Return<false>, True<false>,
          Try<false>, While<false>, With<false>, Yield<false>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
