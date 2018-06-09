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

#include "object/number/details.hpp"

#include <gsl/gsl>

#include "object/number/compare.hpp"
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
        static PositiveValue positive(Natural &&value) {
          while (!value.value.empty()) {
            if (value.value.back() != 0) {
              break;
            }
            value.value.pop_back();
          }
          if (value.value.empty()) {
            return Base{};
          }
          if (value.value.size() == 1) {
            return Base{value.value[0]};
          }
          value.value.shrink_to_fit();
          return value;
        }

        static IntegerValue integer(Natural &&value) {
          return std::visit([](auto &&v) { return IntegerValue(v); }, positive(std::move(value)));
        }

        static IntegerValue integer(Positive &&value) {
          return std::visit([](auto &&v) { return IntegerValue(v); }, value.value);
        }

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

        template <typename Left, typename Right>
        static Rational to_rational_pos(Left &&left, Right &&right) {
          return std::visit(
              [](auto &&a, auto &&b) { return to_rational(a, b); },
              integer(std::move(left)), integer(std::move(right)));
        }

        template <typename Left, typename Right>
        static Rational to_rational_neg(Left &&left, Right &&right) {
          return std::visit(
              [](auto &&a, auto &&b) { return to_rational(-a, b); },
              integer(std::move(left)), integer(std::move(right)));
        }

        static RealValue reduce_pos(Positive left, Positive right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational_pos(left, right);
          }
          if (right == 1u) {
            return left;
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

        static RealValue reduce_neg(Positive left, Positive right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational_neg(left, right);
          }
          if (right == 1u) {
            return -left;
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
        RealValue reduce(Left &&left, Right &&right) {
          if (left == 0u) {
            return left;
          }
          if (left == right) {
            return Base{1u};
          }
          if (left == 1u) {
            return Rational{left, right};
          }
          if (right == 0u) {
            return Rational{left, right};
          }
          if (right == 1u) {
            return left;
          }
          if ((left < 0) ^ (right < 0)) {
            return reduce_neg(Positive(+left), Positive(+right));
          }
          return reduce_pos(Positive(+left), Positive(+right));
        }

        static RealValue real(Natural natural) {
          return std::visit([](auto &&v) { return RealValue(v); }, positive(std::move(natural)));
        }

        static RealValue real(Positive positive) {
          return std::visit(
              [](const auto &value) { return RealValue(value); },
              positive.value);
        }

        static RealValue real(Negative negative) { return negative; }

        static RealValue real(Integer integer) {
          return std::visit(
              [](const auto &value) { return RealValue(value); },
              integer.value);
        }

        static RealValue real(Rational rational) {
          return std::visit(
              [](const auto &numerator, const auto &denominator) {
                return reduce(numerator, denominator);
              },
              rational.numerator, rational.denominator);
        }

        Positive::Positive(Base base) : value(base) {}
        Positive::Positive(Natural natural)
            : value(positive(std::move(natural))) {}

        Negative::Negative(Base base) : value(base) {}
        Negative::Negative(Natural natural)
            : value(positive(std::move(natural))) {}
        Negative::Negative(Positive positive)
            : value(std::move(positive.value)) {}

        Integer::Integer(Base base) : value(base) {}
        Integer::Integer(Natural natural)
            : value(integer(std::move(natural))) {}
        Integer::Integer(Positive positive)
            : value(integer(std::move(positive))) {}
        Integer::Integer(Negative negative) : value(std::move(negative)) {}

        Real::Real(Base base) : value(base) {}
        Real::Real(Natural natural)
            : value(real(std::move(natural))) {}
        Real::Real(Positive positive)
            : value(real(std::move(positive))) {}
        Real::Real(Negative negative) : value(real(std::move(negative))) {}
        Real::Real(Integer integer) : value(real(std::move(integer))) {}
        Real::Real(Rational rational) : value(real(std::move(rational))) {}
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
