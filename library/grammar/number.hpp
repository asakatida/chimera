//! parse definitions for number tokens.

#pragma once

#include "asdl/asdl.hpp"
#include "grammar/rules.hpp"
#include "grammar/whitespace.hpp"
#include "object/object.hpp"

#include <cstdint>
#include <numeric>
#include <vector>

namespace chimera::library::grammar {
  namespace token {
    struct NullTransaction {
      static void commit() noexcept {}
    };
    struct NumberHolder {
      template <std::uint8_t Base, typename Input, typename... Args>
      void apply(const Input &input, Args &&.../*args*/) {
        std::uint8_t mantisa = 0;
        switch (Base) {
          case 2:
            mantisa = 63;
            break;
          case 8:
            mantisa = 19;
            break;
          case 10:
            mantisa = 17;
            break;
          case 16:
            mantisa = 15;
            break;
          default:
            Ensures(false);
        }
        for (auto parse_string = input.string(); !parse_string.empty();
             parse_string.erase(0, mantisa)) {
          std::size_t len = 0;
          auto section = parse_string.substr(0, mantisa);
          auto parsed_number = std::stoul(section, &len, Base);
          Ensures(len == section.size());
          number *= object::Number(Base).pow(object::Number(len));
          number += object::Number(parsed_number);
        }
      }
      object::Number number = object::Number(0U);
    };
    struct Nonzerodigit : seq<range<'1', '9'>, rep_opt<18, range<'0', '9'>>> {};
    template <>
    struct Action<Nonzerodigit> {
      template <typename Input, typename Top, typename... Args>
      static void apply(const Input &input, Top &&top, Args &&.../*args*/) {
        top.template apply<10>(input);
      }
    };
    struct Digit : seq<range<'0', '9'>, rep_opt<18, range<'0', '9'>>> {};
    template <>
    struct Action<Digit> {
      template <typename Input, typename Top, typename... Args>
      static void apply(const Input &input, Top &&top, Args &&.../*args*/) {
        top.template apply<10>(input);
      }
    };
    struct Bindigit : seq<range<'0', '1'>, rep_opt<63, range<'0', '1'>>> {};
    template <>
    struct Action<Bindigit> {
      template <typename Input, typename Top, typename... Args>
      static void apply(const Input &input, Top &&top, Args &&.../*args*/) {
        top.template apply<2>(input);
      }
    };
    struct Octdigit : seq<range<'0', '7'>, rep_opt<31, range<'0', '7'>>> {};
    template <>
    struct Action<Octdigit> {
      template <typename Input, typename Top, typename... Args>
      static void apply(const Input &input, Top &&top, Args &&.../*args*/) {
        top.template apply<8>(input);
      }
    };
    struct Hexdigit : seq<ranges<'0', '9', 'a', 'f', 'A', 'F'>,
                          rep_opt<15, ranges<'0', '9', 'a', 'f', 'A', 'F'>>> {};
    template <>
    struct Action<Hexdigit> {
      template <typename Input, typename Top, typename... Args>
      static void apply(const Input &input, Top &&top, Args &&.../*args*/) {
        top.template apply<16>(input);
      }
    };
    using DecIntegerNonZeroDigit =
        if_must<Nonzerodigit, star<opt<one<'_'>>, Digit, discard>>;
    using DecIntegerZeroDigit =
        if_must<one<'0'>, star<opt<one<'_'>>, one<'0'>, discard>>;
    using Decinteger = sor<DecIntegerNonZeroDigit, DecIntegerZeroDigit>;
    using BinStart = seq<one<'0'>, one<'b', 'B'>>;
    using Bininteger =
        if_must<BinStart, plus<opt<one<'_'>>, Bindigit, discard>>;
    using OctStart = seq<one<'0'>, one<'o', 'O'>>;
    using Octinteger =
        if_must<OctStart, plus<opt<one<'_'>>, Octdigit, discard>>;
    using HexStart = seq<one<'0'>, one<'x', 'X'>>;
    using Hexinteger =
        if_must<HexStart, plus<opt<one<'_'>>, Hexdigit, discard>>;
    using Integer = sor<Bininteger, Octinteger, Hexinteger, Decinteger>;
    using Digitpart = plus<opt<one<'_'>>, Digit, discard>;
    using Fraction = if_must<one<'.'>, Digitpart, discard>;
    struct ExponentNegative : if_must<one<'-'>, Digitpart> {
      struct Transform : NumberHolder {
        template <typename Top>
        void finalize(Transform & /*unused*/, Top &&top) {
          top.number = -number;
        }
      };
    };
    struct Exponent
        : if_must<one<'e', 'E'>,
                  sor<seq<opt<one<'+'>>, Digitpart>, ExponentNegative>> {
      struct Transform : NumberHolder {
        template <typename Top>
        void finalize(Transform & /*unused*/, Top &&top) {
          top.number *= object::Number(10).pow(number);
        }
        [[nodiscard]] static auto transaction() noexcept -> NullTransaction {
          return {};
        }
      };
    };
    using Pointfloat =
        sor<seq<opt<Digitpart>, Fraction>, if_must<Digitpart, one<'.'>>>;
    using Exponentfloat = if_must<sor<Digitpart, Pointfloat>, Exponent>;
    struct Floatnumber : sor<Pointfloat, Exponentfloat> {
      struct Transform : NumberHolder {
        object::Number denominator = object::Number(1);
        template <typename Top>
        void finalize(Transform & /*unused*/, Top &&top) {
          top.number += number / object::Number(10).pow(denominator);
        }
        template <std::uint8_t Base, typename Input, typename... Args>
        void apply(const Input &input, Args &&.../*args*/) {
          denominator += object::Number(input.size());
          NumberHolder::apply<Base>(input);
        }
        [[nodiscard]] static auto transaction() noexcept -> NullTransaction {
          return {};
        }
      };
    };
    struct Imagnumber : seq<sor<Floatnumber, Digitpart>, one<'j', 'J'>> {
      struct Transform : NumberHolder {
        template <typename Top>
        void finalize(Transform & /*unused*/, Top &&top) {
          top.number = number.imag();
        }
        [[nodiscard]] static auto transaction() noexcept -> NullTransaction {
          return {};
        }
      };
    };
    struct Numberliteral : sor<Imagnumber, Floatnumber, Integer> {
      struct Transform : NumberHolder, rules::VariantCapture<asdl::ExprImpl> {};
    };
    template <>
    struct Action<Numberliteral> {
      template <typename Top, typename... Args>
      static void apply0(Top &&top, Args &&.../*args*/) {
        top.push(object::Object(std::move(top.number), {}));
      }
    };
  } // namespace token
  template <flags::Flag Option>
  struct NUMBER : token::Token<Option, token::Numberliteral> {};
} // namespace chimera::library::grammar
