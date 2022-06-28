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

#include "asdl/asdl.hpp"
#include "grammar/identifier.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"

namespace chimera::library::grammar {
  namespace token {
    template <flags::Flag Option, typename String>
    using Key = Token<Option, seq<String, not_at<XidContinue>>>;
    template <flags::Flag Option>
    struct And : Key<Option, String<'a', 'n', 'd'>> {};
    template <flags::Flag Option>
    struct As : Key<Option, String<'a', 's'>> {};
    template <flags::Flag Option>
    struct Assert : Key<Option, String<'a', 's', 's', 'e', 'r', 't'>> {};
    template <flags::Flag Option>
    struct Async : Key<Option, String<'a', 's', 'y', 'n', 'c'>> {};
    template <flags::Flag Option>
    struct Await : Key<Option, String<'a', 'w', 'a', 'i', 't'>> {};
    template <flags::Flag Option>
    struct Break : Key<Option, String<'b', 'r', 'e', 'a', 'k'>> {};
    template <flags::Flag Option>
    struct Class : Key<Option, String<'c', 'l', 'a', 's', 's'>> {};
    template <flags::Flag Option>
    struct Continue
        : Key<Option, String<'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'>> {};
    template <flags::Flag Option>
    struct Def : Key<Option, String<'d', 'e', 'f'>> {};
    template <flags::Flag Option>
    struct Del : Key<Option, String<'d', 'e', 'l'>> {};
    template <flags::Flag Option>
    struct Elif : Key<Option, String<'e', 'l', 'i', 'f'>> {};
    template <flags::Flag Option>
    struct Else : Key<Option, String<'e', 'l', 's', 'e'>> {};
    template <flags::Flag Option>
    struct Except : Key<Option, String<'e', 'x', 'c', 'e', 'p', 't'>> {};
    template <flags::Flag Option>
    struct False : Key<Option, String<'F', 'a', 'l', 's', 'e'>> {
      struct Transform {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::NameConstant{asdl::NameConstant::FALSE});
        }
      };
    };
    template <flags::Flag Option>
    struct Finally : Key<Option, String<'f', 'i', 'n', 'a', 'l', 'l', 'y'>> {};
    template <flags::Flag Option>
    struct For : Key<Option, String<'f', 'o', 'r'>> {};
    template <flags::Flag Option>
    struct From : Key<Option, String<'f', 'r', 'o', 'm'>> {};
    template <flags::Flag Option>
    struct Global : Key<Option, String<'g', 'l', 'o', 'b', 'a', 'l'>> {};
    template <flags::Flag Option>
    struct If : Key<Option, String<'i', 'f'>> {};
    template <flags::Flag Option>
    struct Import : Key<Option, String<'i', 'm', 'p', 'o', 'r', 't'>> {};
    template <flags::Flag Option>
    using InImpl = Key<Option, String<'i', 'n'>>;
    template <flags::Flag Option>
    struct In : seq<InImpl<Option>> {};
    template <flags::Flag Option>
    struct InOp : seq<InImpl<Option>> {};
    template <flags::Flag Option>
    struct Action<InOp<Option>> : ConstantToken<asdl::CompareExpr::Op::IN> {};
    template <flags::Flag Option>
    using IsImpl = Key<Option, String<'i', 's'>>;
    template <flags::Flag Option>
    struct Lambda : Key<Option, String<'l', 'a', 'm', 'b', 'd', 'a'>> {};
    template <flags::Flag Option>
    struct None : Key<Option, String<'N', 'o', 'n', 'e'>> {
      struct Transform {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::NameConstant{asdl::NameConstant::NONE});
        }
      };
    };
    template <flags::Flag Option>
    struct Nonlocal
        : Key<Option, String<'n', 'o', 'n', 'l', 'o', 'c', 'a', 'l'>> {};
    template <flags::Flag Option>
    using NotImpl = Key<Option, String<'n', 'o', 't'>>;
    template <flags::Flag Option>
    struct NotIn : seq<NotImpl<Option>, InImpl<Option>> {};
    template <flags::Flag Option>
    struct Action<NotIn<Option>>
        : ConstantToken<asdl::CompareExpr::Op::NOT_IN> {};
    template <flags::Flag Option>
    struct Is : seq<IsImpl<Option>, not_at<NotImpl<Option>>> {};
    template <flags::Flag Option>
    struct Action<Is<Option>> : ConstantToken<asdl::CompareExpr::Op::IS> {};
    template <flags::Flag Option>
    struct IsNot : seq<IsImpl<Option>, NotImpl<Option>> {};
    template <flags::Flag Option>
    struct Action<IsNot<Option>>
        : ConstantToken<asdl::CompareExpr::Op::IS_NOT> {};
    template <flags::Flag Option>
    struct Not : seq<NotImpl<Option>, not_at<InImpl<Option>>> {};
    template <flags::Flag Option>
    struct Or : Key<Option, String<'o', 'r'>> {};
    template <flags::Flag Option>
    struct Pass : Key<Option, String<'p', 'a', 's', 's'>> {};
    template <flags::Flag Option>
    struct Raise : Key<Option, String<'r', 'a', 'i', 's', 'e'>> {};
    template <flags::Flag Option>
    struct Return : Key<Option, String<'r', 'e', 't', 'u', 'r', 'n'>> {};
    template <flags::Flag Option>
    struct True : Key<Option, String<'T', 'r', 'u', 'e'>> {
      struct Transform {
        template <typename Outer>
        void success(Outer &&outer) {
          outer.push(asdl::NameConstant{asdl::NameConstant::TRUE});
        }
      };
    };
    template <flags::Flag Option>
    struct Try : Key<Option, String<'t', 'r', 'y'>> {};
    template <flags::Flag Option>
    struct While : Key<Option, String<'w', 'h', 'i', 'l', 'e'>> {};
    template <flags::Flag Option>
    struct With : Key<Option, String<'w', 'i', 't', 'h'>> {};
    template <flags::Flag Option>
    struct Yield : Key<Option, String<'y', 'i', 'e', 'l', 'd'>> {};
    using Keywords =
        sor<And<0>, As<0>, Assert<0>, Break<0>, Class<0>, Continue<0>, Def<0>,
            Del<0>, Elif<0>, Else<0>, Except<0>, False<0>, Finally<0>, For<0>,
            From<0>, Global<0>, If<0>, Import<0>, InImpl<0>, IsImpl<0>,
            Lambda<0>, None<0>, Nonlocal<0>, NotImpl<0>, Or<0>, Pass<0>,
            Raise<0>, Return<0>, True<0>, Try<0>, While<0>, With<0>, Yield<0>>;
  } // namespace token
  using token::And;
  using token::As;
  using token::Assert;
  using token::Async;
  using token::Await;
  using token::Break;
  using token::Class;
  using token::Continue;
  using token::Def;
  using token::Del;
  using token::Elif;
  using token::Else;
  using token::Except;
  using token::False;
  using token::Finally;
  using token::For;
  using token::From;
  using token::Global;
  using token::If;
  using token::Import;
  using token::In;
  using token::InOp;
  using token::Is;
  using token::IsNot;
  using token::Keywords;
  using token::Lambda;
  using token::None;
  using token::Nonlocal;
  using token::Not;
  using token::NotIn;
  using token::Or;
  using token::Pass;
  using token::Raise;
  using token::Return;
  using token::True;
  using token::Try;
  using token::While;
  using token::With;
  using token::Yield;
} // namespace chimera::library::grammar
