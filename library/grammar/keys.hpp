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

//! parse definitions for keyword tokens.

#pragma once

#include <tao/pegtl.hpp>

#include "grammar/identifier.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      template <bool Implicit, typename String>
      using Key = Token<Implicit, String, NotAt<XidContinue>>;
      template <bool Implicit>
      struct And : Key<Implicit, String<'a', 'n', 'd'>> {};
      template <bool Implicit>
      struct As : Key<Implicit, String<'a', 's'>> {};
      template <bool Implicit>
      struct Assert : Key<Implicit, String<'a', 's', 's', 'e', 'r', 't'>> {};
      template <bool Implicit>
      struct Async : Key<Implicit, String<'a', 's', 'y', 'n', 'c'>> {};
      template <bool Implicit>
      struct Await : Key<Implicit, String<'a', 'w', 'a', 'i', 't'>> {};
      template <bool Implicit>
      struct Break : Key<Implicit, String<'b', 'r', 'e', 'a', 'k'>> {};
      template <bool Implicit>
      struct Class : Key<Implicit, String<'c', 'l', 'a', 's', 's'>> {};
      template <bool Implicit>
      struct Continue
          : Key<Implicit, String<'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>> {};
      template <bool Implicit>
      struct Def : Key<Implicit, String<'d', 'e', 'f'>> {};
      template <bool Implicit>
      struct Del : Key<Implicit, String<'d', 'e', 'l'>> {};
      template <bool Implicit>
      struct Elif : Key<Implicit, String<'e', 'l', 'i', 'f'>> {};
      template <bool Implicit>
      struct Else : Key<Implicit, String<'e', 'l', 's', 'e'>> {};
      template <bool Implicit>
      struct Except : Key<Implicit, String<'e', 'x', 'c', 'e', 'p', 't'>> {};
      template <bool Implicit>
      struct False : Key<Implicit, String<'F', 'a', 'l', 's', 'e'>> {};
      template <bool Implicit>
      struct ChimeraActions<False<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::FALSE}});
        }
      };
      template <bool Implicit>
      struct Finally
          : Key<Implicit, String<'f', 'i', 'n', 'a', 'l', 'l', 'y'>> {};
      template <bool Implicit>
      struct For : Key<Implicit, String<'f', 'o', 'r'>> {};
      template <bool Implicit>
      struct From : Key<Implicit, String<'f', 'r', 'o', 'm'>> {};
      template <bool Implicit>
      struct Global : Key<Implicit, String<'g', 'l', 'o', 'b', 'a', 'l'>> {};
      template <bool Implicit>
      struct If : Key<Implicit, String<'i', 'f'>> {};
      template <bool Implicit>
      struct Import : Key<Implicit, String<'i', 'm', 'p', 'o', 'r', 't'>> {};
      template <bool Implicit>
      struct InImpl : Key<Implicit, String<'i', 'n'>> {};
      template <bool Implicit>
      struct In : Seq<InImpl<Implicit>> {};
      template <bool Implicit>
      struct InOp : Seq<InImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<InOp<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IN);
        }
      };
      template <bool Implicit>
      struct IsImpl : Key<Implicit, String<'i', 's'>> {};
      template <bool Implicit>
      struct Lambda : Key<Implicit, String<'l', 'a', 'm', 'b', 'd', 'a'>> {};
      template <bool Implicit>
      struct None : Key<Implicit, String<'N', 'o', 'n', 'e'>> {};
      template <bool Implicit>
      struct ChimeraActions<None<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::NONE}});
        }
      };
      template <bool Implicit>
      struct Nonlocal
          : Key<Implicit, String<'n', 'o', 'n', 'l', 'o', 'c', 'a', 'l'>> {};
      template <bool Implicit>
      struct NotImpl : Key<Implicit, String<'n', 'o', 't'>> {};
      template <bool Implicit>
      struct NotIn : Seq<NotImpl<Implicit>, InImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<NotIn<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::NOT_IN);
        }
      };
      template <bool Implicit>
      struct Is : Seq<IsImpl<Implicit>, NotAt<NotImpl<false>>> {};
      template <bool Implicit>
      struct ChimeraActions<Is<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IS);
        }
      };
      template <bool Implicit>
      struct IsNot : Seq<IsImpl<Implicit>, NotImpl<Implicit>> {};
      template <bool Implicit>
      struct ChimeraActions<IsNot<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(asdl::CompareExpr::Op::IS_NOT);
        }
      };
      template <bool Implicit>
      struct Not : Seq<NotImpl<Implicit>, NotAt<InImpl<false>>> {};
      template <bool Implicit>
      struct Or : Key<Implicit, String<'o', 'r'>> {};
      template <bool Implicit>
      struct Pass : Key<Implicit, String<'p', 'a', 's', 's'>> {};
      template <bool Implicit>
      struct Raise : Key<Implicit, String<'r', 'a', 'i', 's', 'e'>> {};
      template <bool Implicit>
      struct Return : Key<Implicit, String<'r', 'e', 't', 'u', 'r', 'n'>> {};
      template <bool Implicit>
      struct True : Key<Implicit, String<'T', 'r', 'u', 'e'>> {};
      template <bool Implicit>
      struct ChimeraActions<True<Implicit>> {
        template <typename ProcessContext, typename Stack>
        static void apply0(ProcessContext && /*processContext*/,
                           Stack &&stack) {
          stack.push(
              asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::TRUE}});
        }
      };
      template <bool Implicit>
      struct Try : Key<Implicit, String<'t', 'r', 'y'>> {};
      template <bool Implicit>
      struct While : Key<Implicit, String<'w', 'h', 'i', 'l', 'e'>> {};
      template <bool Implicit>
      struct With : Key<Implicit, String<'w', 'i', 't', 'h'>> {};
      template <bool Implicit>
      struct Yield : Key<Implicit, String<'y', 'i', 'e', 'l', 'd'>> {};
      using Keywords =
          Sor<And<false>, As<false>, Assert<false>, Break<false>, Class<false>,
              Continue<false>, Def<false>, Del<false>, Elif<false>, Else<false>,
              Except<false>, False<false>, Finally<false>, For<false>,
              From<false>, Global<false>, If<false>, Import<false>,
              InImpl<false>, IsImpl<false>, Lambda<false>, None<false>,
              Nonlocal<false>, NotImpl<false>, Or<false>, Pass<false>,
              Raise<false>, Return<false>, True<false>, Try<false>,
              While<false>, With<false>, Yield<false>>;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
