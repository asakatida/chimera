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

#include "asdl/asdl.hpp"
#include "grammar/identifier.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace key {
        template <auto Asdl>
        struct Transform {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(Asdl);
          }
        };
        template <typename Option, typename String>
        using Key = Token<Option, String, NotAt<name::XidContinue>>;
        template <typename Option>
        using And = Key<Option, String<'a', 'n', 'd'>>;
        template <typename Option>
        using As = Key<Option, String<'a', 's'>>;
        template <typename Option>
        using Assert = Key<Option, String<'a', 's', 's', 'e', 'r', 't'>>;
        template <typename Option>
        using Async = Key<Option, String<'a', 's', 'y', 'n', 'c'>>;
        template <typename Option>
        using Await = Key<Option, String<'a', 'w', 'a', 'i', 't'>>;
        template <typename Option>
        using Break = Key<Option, String<'b', 'r', 'e', 'a', 'k'>>;
        template <typename Option>
        using Class = Key<Option, String<'c', 'l', 'a', 's', 's'>>;
        template <typename Option>
        using Continue =
            Key<Option, String<'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>>;
        template <typename Option>
        using Def = Key<Option, String<'d', 'e', 'f'>>;
        template <typename Option>
        using Del = Key<Option, String<'d', 'e', 'l'>>;
        template <typename Option>
        using Elif = Key<Option, String<'e', 'l', 'i', 'f'>>;
        template <typename Option>
        using Else = Key<Option, String<'e', 'l', 's', 'e'>>;
        template <typename Option>
        using Except = Key<Option, String<'e', 'x', 'c', 'e', 'p', 't'>>;
        template <typename Option>
        struct False : Key<Option, String<'F', 'a', 'l', 's', 'e'>> {
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(asdl::ExprImpl{
                  asdl::NameConstant{asdl::NameConstant::FALSE}});
            }
          };
        };
        template <typename Option>
        using Finally = Key<Option, String<'f', 'i', 'n', 'a', 'l', 'l', 'y'>>;
        template <typename Option>
        using For = Key<Option, String<'f', 'o', 'r'>>;
        template <typename Option>
        using From = Key<Option, String<'f', 'r', 'o', 'm'>>;
        template <typename Option>
        using Global = Key<Option, String<'g', 'l', 'o', 'b', 'a', 'l'>>;
        template <typename Option>
        using If = Key<Option, String<'i', 'f'>>;
        template <typename Option>
        using Import = Key<Option, String<'i', 'm', 'p', 'o', 'r', 't'>>;
        template <typename Option>
        using InImpl = Key<Option, String<'i', 'n'>>;
        template <typename Option>
        using In = Seq<InImpl<Option>>;
        template <typename Option>
        struct InOp : Seq<InImpl<Option>> {
          using Transform = Transform<asdl::CompareExpr::Op::IN>;
        };
        template <typename Option>
        using IsImpl = Key<Option, String<'i', 's'>>;
        template <typename Option>
        using Lambda = Key<Option, String<'l', 'a', 'm', 'b', 'd', 'a'>>;
        template <typename Option>
        struct None : Key<Option, String<'N', 'o', 'n', 'e'>> {
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(
                  asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::NONE}});
            }
          };
        };
        template <typename Option>
        using Nonlocal =
            Key<Option, String<'n', 'o', 'n', 'l', 'o', 'c', 'a', 'l'>>;
        template <typename Option>
        using NotImpl = Key<Option, String<'n', 'o', 't'>>;
        template <typename Option>
        struct NotIn : Seq<NotImpl<Option>, InImpl<Option>> {
          using Transform = Transform<asdl::CompareExpr::Op::NOT_IN>;
        };
        template <typename Option>
        struct Is : Seq<IsImpl<Option>, NotAt<NotImpl<Option>>> {
          using Transform = Transform<asdl::CompareExpr::Op::IS>;
        };
        template <typename Option>
        struct IsNot : Seq<IsImpl<Option>, NotImpl<Option>> {
          using Transform = Transform<asdl::CompareExpr::Op::IS_NOT>;
        };
        template <typename Option>
        using Not = Seq<NotImpl<Option>, NotAt<InImpl<Option>>>;
        template <typename Option>
        using Or = Key<Option, String<'o', 'r'>>;
        template <typename Option>
        using Pass = Key<Option, String<'p', 'a', 's', 's'>>;
        template <typename Option>
        using Raise = Key<Option, String<'r', 'a', 'i', 's', 'e'>>;
        template <typename Option>
        using Return = Key<Option, String<'r', 'e', 't', 'u', 'r', 'n'>>;
        template <typename Option>
        struct True : Key<Option, String<'T', 'r', 'u', 'e'>> {
          struct Transform {
            template <typename Outer>
            void success(Outer &&outer) {
              outer.push(
                  asdl::ExprImpl{asdl::NameConstant{asdl::NameConstant::TRUE}});
            }
          };
        };
        template <typename Option>
        using Try = Key<Option, String<'t', 'r', 'y'>>;
        template <typename Option>
        using While = Key<Option, String<'w', 'h', 'i', 'l', 'e'>>;
        template <typename Option>
        using With = Key<Option, String<'w', 'i', 't', 'h'>>;
        template <typename Option>
        using Yield = Key<Option, String<'y', 'i', 'e', 'l', 'd'>>;
        template <typename Option>
        using Keywords =
            Sor<And<Option>, As<Option>, Assert<Option>, Break<Option>,
                Class<Option>, Continue<Option>, Def<Option>, Del<Option>,
                Elif<Option>, Else<Option>, Except<Option>, False<Option>,
                Finally<Option>, For<Option>, From<Option>, Global<Option>,
                If<Option>, Import<Option>, InImpl<Option>, IsImpl<Option>,
                Lambda<Option>, None<Option>, Nonlocal<Option>, NotImpl<Option>,
                Or<Option>, Pass<Option>, Raise<Option>, Return<Option>,
                True<Option>, Try<Option>, While<Option>, With<Option>,
                Yield<Option>>;
      } // namespace key
      using key::And;
      using key::As;
      using key::Assert;
      using key::Async;
      using key::Await;
      using key::Break;
      using key::Class;
      using key::Continue;
      using key::Def;
      using key::Del;
      using key::Elif;
      using key::Else;
      using key::Except;
      using key::False;
      using key::Finally;
      using key::For;
      using key::From;
      using key::Global;
      using key::If;
      using key::Import;
      using key::In;
      using key::InOp;
      using key::Is;
      using key::IsNot;
      using key::Keywords;
      using key::Lambda;
      using key::None;
      using key::Nonlocal;
      using key::Not;
      using key::NotIn;
      using key::Or;
      using key::Pass;
      using key::Raise;
      using key::Return;
      using key::True;
      using key::Try;
      using key::While;
      using key::With;
      using key::Yield;
    } // namespace grammar
  }   // namespace library
} // namespace chimera
