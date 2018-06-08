// Copyright (c) 2018 Asa Katida <github@holomaplefeline.net>
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

#include "object/number/simplify.hpp"

#include <gsl/gsl>

#include "object/number/compare.hpp"
#include "object/number/details.hpp"
#include "object/number/floor_div.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/positive.hpp"
#include "object/number/right_shift.hpp"
#include "object/number/sub.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static bool even(std::uint64_t i) { return (i & 1u) == 0u; }
        static bool even(Base i) { return even(i.value); }
        static bool even(const Natural &i) { return even(i.value[0]); }
        static bool even(const Positive &i) {
          return std::visit([](const auto &value) { return even(value); },
                            i.value);
        }

        static Rational to_rational(Base left, Base right) {
          return Rational{left, right};
        }

        static Rational to_rational(Base left, const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(Base left, const Negative &right) {
          return Rational{left, right};
        }

        static Rational to_rational(Base /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Natural &left, Base right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural &left, const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural &left,
                                    const Negative &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural & /*left*/,
                                    const Rational & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Negative &left, Base right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Negative &left,
                                    const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Negative &left,
                                    const Negative &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Negative & /*left*/,
                                    const Rational & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Positive &left, Base right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Positive &left,
                                    const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Positive &left,
                                    const Negative &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Positive & /*left*/,
                                    const Rational & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Rational & /*left*/, Base /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Rational & /*left*/,
                                    const Natural & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Rational & /*left*/,
                                    const Negative & /*right*/) {
          Expects(false);
        }

        static Rational to_rational(const Rational & /*left*/,
                                    const Rational & /*right*/) {
          Expects(false);
        }

        template <typename Left, typename Right>
        static Rational to_rational_pos(Left &&left, Right &&right) {
          return std::visit(
              [](auto &&a, auto &&b) { return to_rational(a, b); },
              simplify(left), simplify(right));
        }

        template <typename Left, typename Right>
        static Rational to_rational_neg(Left &&left, Right &&right) {
          return std::visit(
              [](auto &&a, auto &&b) { return to_rational(-a, b); },
              simplify(left), simplify(right));
        }

        static NumberValue reduce_pos(Positive left, Positive right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational_pos(left, right);
          }
          if (right == 1u) {
            return simplify(left);
          }
          auto aPrime = left, bPrime = right;
          while (!(aPrime == bPrime)) {
            if (even(aPrime)) {
              aPrime = aPrime >> 1u;
            } else if (even(bPrime)) {
              bPrime = bPrime >> 1u;
            } else if (bPrime < aPrime) {
              aPrime = +(aPrime - bPrime) >> 1u;
            } else {
              bPrime = +(bPrime - aPrime) >> 1u;
            }
          }
          if (aPrime == 1u) {
            return to_rational_pos(left, right);
          }
          if (left == aPrime) {
            return to_rational_pos(Base{1u}, floor_div(right, aPrime));
          }
          if (right == aPrime) {
            return floor_div(left, aPrime);
          }
          return to_rational_pos(floor_div(left, aPrime),
                                 floor_div(right, aPrime));
        }

        static NumberValue reduce_neg(Positive left, Positive right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational_neg(left, right);
          }
          if (right == 1u) {
            return simplify(-left);
          }
          auto aPrime = left, bPrime = right;
          while (!(aPrime == bPrime)) {
            if (even(aPrime)) {
              aPrime = aPrime >> 1u;
            } else if (even(bPrime)) {
              bPrime = bPrime >> 1u;
            } else if (bPrime < aPrime) {
              aPrime = +(aPrime - bPrime) >> 1u;
            } else {
              bPrime = +(bPrime - aPrime) >> 1u;
            }
          }
          if (aPrime == 1u) {
            return to_rational_neg(left, right);
          }
          if (left == aPrime) {
            return to_rational_neg(Base{1u}, floor_div(right, aPrime));
          }
          if (right == aPrime) {
            return -floor_div(left, aPrime);
          }
          return to_rational_neg(floor_div(left, aPrime),
                                 floor_div(right, aPrime));
        }

        template <typename Left, typename Right>
        NumberValue reduce(Left &&left, Right &&right) {
          if (left == 0u) {
            return {};
          }
          if (left == right) {
            return Base{1u};
          }
          if (left == 1u) {
            return Rational{left, right};
          }
          if (right == 0u) {
            return Rational{left, {}};
          }
          if (right == 1u) {
            return simplify(left);
          }
          if ((left < 0) ^ (right < 0)) {
            return reduce_neg(Positive(+left), Positive(+right));
          }
          return reduce_pos(Positive(+left), Positive(+right));
        }

        NumberValue simplify(Base base) { return base; }
        NumberValue simplify(Natural natural) {
          while (!natural.value.empty()) {
            if (natural.value.back() != 0) {
              break;
            }
            natural.value.pop_back();
          }
          if (natural.value.empty()) {
            return Base{};
          }
          if (natural.value.size() == 1) {
            return Base{natural.value[0]};
          }
          natural.value.shrink_to_fit();
          return natural;
        }
        NumberValue simplify(Positive positive) {
          return std::visit(
              [](const auto &value) { return NumberValue(value); },
              positive.value);
        }
        NumberValue simplify(Negative negative) { return negative; }
        NumberValue simplify(Integer integer) {
          return std::visit(
              [](const auto &value) { return NumberValue(value); },
              integer.value);
        }
        NumberValue simplify(Rational rational) {
          return std::visit(
              [](const auto &numerator, const auto &denominator) {
                return reduce(numerator, denominator);
              },
              rational.numerator, rational.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
