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

#include "object/number/div.hpp"

#include <tuple>
#include <type_traits>
#include <vector>

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static bool even(const std::uint64_t &i) { return (i & 1) == 0u; }
        static bool even(const Base &i) { return even(i.value); }
        static bool even(const Natural &i) { return even(i.value[0]); }
        static bool even(const Integer & /*i*/) { Ensures(false); }
        static bool even(const Rational & /*i*/) { Ensures(false); }
        static bool even(const Number &i) {
          return std::visit([](auto &&value) { return even(value); }, i.value);
        }

        static RationalValue floor_div(const Natural &a, const Base &b) {
          auto value = a;
          Carryover carryover{};
          for (auto it = value.value.rbegin(); it != value.value.rend(); ++it) {
            carryover.result = *it;
            carryover = div(carryover, b.value);
            *it = carryover.result;
          }
          while (value.value.back() == 0) {
            value.value.pop_back();
            if (value.value.empty()) {
              return Base{};
            }
          }
          if (value.value.size() == 1) {
            return Base{value.value[0]};
          }
          value.value.shrink_to_fit();
          return value;
        }
        static RationalValue floor_div(const Natural &a, const Natural &b) {
          return Base{a.value[0] / b.value[0]};
        }

        static Number reduce(const Base &a, const Base &b, const Base &g) {
          return Number{
              Rational{Base{a.value / g.value}, Base{b.value / g.value}}};
        }
        static Number reduce(const Base &a, const Natural &b, const Base &g) {
          return Number{Rational{Base{a.value / g.value}, floor_div(b, g)}};
        }
        static Number reduce(const Natural &a, const Base &b, const Base &g) {
          return Number{Rational{floor_div(a, g), Base{b.value / g.value}}};
        }
        static Number reduce(const Natural &a, const Natural &b,
                             const Base &g) {
          return Number{Rational{floor_div(a, g), floor_div(b, g)}};
        }
        static Number reduce(const Natural &a, const Natural &b,
                             const Natural &g) {
          return Number{Rational{floor_div(a, g), floor_div(b, g)}};
        }

        static Number reduce(const Base & /*a*/, const Base & /*b*/,
                             const Natural & /*g*/) {
          Ensures(false);
        }
        static Number reduce(const Base & /*a*/, const Natural & /*b*/,
                             const Natural & /*g*/) {
          Ensures(false);
        }
        static Number reduce(const Natural & /*a*/, const Base & /*b*/,
                             const Natural & /*g*/) {
          Ensures(false);
        }
        template <typename A, typename B, typename G>
        std::enable_if_t<
            std::disjunction_v<std::is_same<Integer, std::decay_t<A>>,
                               std::is_same<Integer, std::decay_t<B>>,
                               std::is_same<Integer, std::decay_t<G>>,
                               std::is_same<Rational, std::decay_t<A>>,
                               std::is_same<Rational, std::decay_t<B>>,
                               std::is_same<Rational, std::decay_t<G>>>,
            Number>
        reduce(A && /*a*/, B && /*b*/, G && /*g*/) {
          Ensures(false);
        }

        template <typename Left, typename Right>
        Number reduce(Left &&left, Right &&right) {
          Number a{left}, b{right};
          while (even(a) && even(b)) {
            a >>= 1u;
            b >>= 1u;
          }
          auto aPrime = a, bPrime = b;
          while (aPrime != bPrime) {
            if (even(aPrime)) {
              aPrime >>= 1u;
            } else if (even(bPrime)) {
              bPrime >>= 1u;
            } else if (aPrime > bPrime) {
              aPrime = (aPrime - bPrime) >> 1u;
            } else {
              bPrime = (bPrime - aPrime) >> 1u;
            }
          }
          return std::visit([](auto &&aValue, auto &&bValue,
                               auto &&g) { return reduce(aValue, bValue, g); },
                            a.value, b.value, aPrime.value);
        }

        Number operator/(const std::uint64_t &left, const Base &right) {
          if (left % right.value == 0) {
            return Number{Base{left / right.value}};
          }
          return reduce(Base{left}, right);
        }

        Number operator/(const std::uint64_t &left, const Natural &right) {
          return reduce(Base{left}, right);
        }

        Number operator/(const std::uint64_t &left, const Integer &right) {
          return -std::visit([&left](auto &&r) { return left / r; },
                             right.value);
        }

        Number operator/(const std::uint64_t &left, const Rational &right) {
          return std::visit(
              [&left](auto &&rN, auto &&rD) { return (left * rD) / rN; },
              right.numerator, right.denominator);
        }

        Number operator/(const Base &left, const std::uint64_t &right) {
          return reduce(left, Base{right});
        }

        Number operator/(const Base &left, const Base &right) {
          return reduce(left, right);
        }

        Number operator/(const Base &left, const Natural &right) {
          return reduce(left, right);
        }

        Number operator/(const Base &left, const Integer &right) {
          return -std::visit([&left](auto &&r) { return left / r; },
                             right.value);
        }

        Number operator/(const Base &left, const Rational &right) {
          return std::visit(
              [&left](auto &&rN, auto &&rD) { return (left * rD) / rN; },
              right.numerator, right.denominator);
        }

        Number operator/(const Natural &left, const std::uint64_t &right) {
          return reduce(left, Base{right});
        }

        Number operator/(const Natural &left, const Base &right) {
          return reduce(left, right);
        }

        Number operator/(const Natural &left, const Natural &right) {
          return reduce(left, right);
        }

        Number operator/(const Natural &left, const Integer &right) {
          return -std::visit([&left](auto &&r) { return left / r; },
                             right.value);
        }

        Number operator/(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](auto &&rN, auto &&rD) { return (left * rD) / rN; },
              right.numerator, right.denominator);
        }

        Number operator/(const Integer &left, const std::uint64_t &right) {
          return -std::visit([&right](auto &&l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Base &right) {
          return -std::visit([&right](auto &&l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Natural &right) {
          return -std::visit([&right](auto &&l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Integer &right) {
          return std::visit([](auto &&l, auto &&r) { return l / r; },
                            left.value, right.value);
        }

        Number operator/(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](auto &&rN, auto &&rD) { return (left * rD) / rN; },
              right.numerator, right.denominator);
        }

        Number operator/(const Rational &left, const std::uint64_t &right) {
          return std::visit(
              [&right](auto &&lN, auto &&lD) { return lN / (lD * right); },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Base &right) {
          return std::visit(
              [&right](auto &&lN, auto &&lD) { return lN / (lD * right); },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](auto &&lN, auto &&lD) { return lN / (lD * right); },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](auto &&lN, auto &&lD) { return lN / (lD * right); },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Rational &right) {
          return std::visit([](auto &&lN, auto &&lD, auto &&rN,
                               auto &&rD) { return (lN * rD) / (lD * rN); },
                            left.numerator, left.denominator, right.numerator,
                            right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
