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

//! parse definitions for number tokens.

#pragma once

#include <cstdint>
#include <numeric>
#include <vector>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/changes.hpp>

#include "asdl/asdl.hpp"
#include "grammar/control.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace number {
        template <typename Rule>
        struct Actions : tao::pegtl::nothing<Rule> {};
        template <typename Rule>
        struct Control : ControlBase<Rule> {};
        template <typename... Args>
        object::Number ston(Args &&... args) {
          return object::number::number(
              std::stoul(std::forward<Args>(args)...));
        }
        struct Holder {
          object::Number number = object::number::number(0);
          object::Id object_ref = 0;
          template <typename Stack>
          void success(Stack &&stack) {
            stack.push(asdl::ExprImpl{asdl::Constant{object_ref}});
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct FloatHolder {
          object::Number numerator = object::number::number(0);
          object::Number denominator = object::number::number(1);
          template <typename Top>
          void success(Top &&top) {
            top.object =
                numerator / object::number::number(10).pow(denominator) +
                top.object;
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            auto size = object::number::number(in.size());
            denominator = denominator + size;
            numerator = numerator * object::number::number(Base).pow(size) +
                        ston(in.string(), nullptr, Base);
          }
        };
        struct ExponentHolder {
          object::Number number = object::number::number(0);
          template <typename Top>
          void success(Top &&top) {
            top.number = object::number::number(10).pow(number) * top.number;
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct ImaginaryHolder {
          object::Number number = object::number::number(0);
          template <typename Top>
          void success(Top &&top) {
            top.number = number.complex();
          }
          template <std::uint8_t Base, typename Input>
          void apply(const Input &in) {
            number = number * object::number::number(Base).pow(
                                  object::number::number(in.size())) +
                     ston(in.string(), nullptr, Base);
          }
        };
        struct Nonzerodigit
            : tao::pegtl::seq<tao::pegtl::range<'1', '9'>,
                              tao::pegtl::rep_opt<18, tao::pegtl::digit>> {};
        template <>
        struct Actions<Nonzerodigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Digit
            : tao::pegtl::seq<tao::pegtl::digit,
                              tao::pegtl::rep_opt<18, tao::pegtl::digit>> {};
        template <>
        struct Actions<Digit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<10>(in);
          }
        };
        struct Bindigit
            : tao::pegtl::seq<
                  tao::pegtl::range<'0', '1'>,
                  tao::pegtl::rep_opt<63, tao::pegtl::range<'0', '1'>>> {};
        template <>
        struct Actions<Bindigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<2>(in);
          }
        };
        struct Octdigit
            : tao::pegtl::seq<
                  tao::pegtl::range<'0', '7'>,
                  tao::pegtl::rep_opt<31, tao::pegtl::range<'0', '7'>>> {};
        template <>
        struct Actions<Octdigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<8>(in);
          }
        };
        struct Hexdigit
            : tao::pegtl::seq<tao::pegtl::xdigit,
                              tao::pegtl::rep_opt<15, tao::pegtl::xdigit>> {};
        template <>
        struct Actions<Hexdigit> {
          template <typename Input, typename Top>
          static void apply(const Input &in, Top &&top) {
            top.template apply<16>(in);
          }
        };
        using DecIntegerNonZeroDigit = tao::pegtl::seq<
            Nonzerodigit,
            tao::pegtl::star<tao::pegtl::opt<tao::pegtl::one<'_'>>, Digit>>;
        using DecIntegerZeroDigit = tao::pegtl::seq<
            tao::pegtl::one<'0'>,
            tao::pegtl::star<tao::pegtl::opt<tao::pegtl::one<'_'>>,
                             tao::pegtl::one<'0'>>>;
        using Decinteger =
            tao::pegtl::sor<DecIntegerNonZeroDigit, DecIntegerZeroDigit>;
        using Bininteger = tao::pegtl::if_must<
            tao::pegtl::istring<'0', 'b'>,
            tao::pegtl::plus<tao::pegtl::opt<tao::pegtl::one<'_'>>, Bindigit>>;
        using Octinteger = tao::pegtl::if_must<
            tao::pegtl::istring<'0', 'o'>,
            tao::pegtl::plus<tao::pegtl::opt<tao::pegtl::one<'_'>>, Octdigit>>;
        using Hexinteger = tao::pegtl::if_must<
            tao::pegtl::istring<'0', 'x'>,
            tao::pegtl::plus<tao::pegtl::opt<tao::pegtl::one<'_'>>, Hexdigit>>;
        using Integer =
            tao::pegtl::sor<Bininteger, Octinteger, Hexinteger, Decinteger>;

        using Digitpart =
            tao::pegtl::plus<tao::pegtl::opt<tao::pegtl::one<'_'>>, Digit>;
        using Fraction = tao::pegtl::seq<tao::pegtl::one<'.'>, Digitpart>;
        struct ExponentNegative
            : tao::pegtl::seq<tao::pegtl::one<'-'>, Digitpart> {};
        template <>
        struct Actions<ExponentNegative> {
          template <typename Top>
          static void apply0(Top &&top) {
            top.number = -top.number;
          }
        };
        using Exponent = tao::pegtl::if_must<
            tao::pegtl::one<'e', 'E'>,
            tao::pegtl::sor<
                tao::pegtl::seq<tao::pegtl::opt<tao::pegtl::one<'+'>>,
                                Digitpart>,
                ExponentNegative>>;
        using Pointfloat = tao::pegtl::sor<
            tao::pegtl::seq<tao::pegtl::opt<Digitpart>, Fraction>,
            tao::pegtl::seq<Digitpart, tao::pegtl::one<'.'>>>;
        using Exponentfloat =
            tao::pegtl::seq<tao::pegtl::sor<Digitpart, Pointfloat>, Exponent>;
        using Floatnumber = tao::pegtl::sor<Pointfloat, Exponentfloat>;
        struct Imagnumber
            : tao::pegtl::seq<tao::pegtl::sor<Floatnumber, Digitpart>,
                              tao::pegtl::one<'j', 'J'>> {};
        template <>
        struct Control<Imagnumber> : ChangeState<Imagnumber, ImaginaryHolder> {
        };
        struct NumberCreate : tao::pegtl::success {
          template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
                    template <typename...> class Action,
                    template <typename...> class Control, typename Input,
                    typename Top>
          static bool match(Input &&in, Top &&top) {
            top.object_ref =
                in.process_context.insert_constant(std::move(top.number));
            return true;
          }
        };
        struct Numberliteral
            : tao::pegtl::seq<tao::pegtl::sor<Imagnumber, Floatnumber, Integer>,
                              NumberCreate> {};
        template <>
        struct Control<Numberliteral> : ChangeState<Numberliteral, Holder> {};
      } // namespace number
      template <bool Implicit>
      struct NUMBER
          : Token<Implicit, tao::pegtl::action<
                                number::Actions,
                                tao::pegtl::control<number::Control,
                                                    number::Numberliteral>>> {};
    } // namespace grammar
  }   // namespace library
} // namespace chimera
