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
            return Base{0u};
          }
          if (value.value.size() == 1) {
            return Base{value.value[0]};
          }
          value.value.shrink_to_fit();
          return value;
        }

        static IntegerValue integer(Natural &&value) {
          return std::visit([](auto &&v) { return IntegerValue(v); },
                            positive(std::move(value)));
        }

        static IntegerValue integer(Positive &&value) {
          return std::visit([](auto &&v) { return IntegerValue(v); },
                            value.value);
        }

        static bool even(std::uint64_t i) { return (i & 1u) == 0u; }
        static bool even(Base i) { return even(i.value); }
        static bool even(const Natural &i) { return even(i.value[0]); }
        static bool even(const Positive &i) {
          return std::visit([](const auto &value) { return even(value); },
                            i.value);
        }

        static Rational to_rational(bool sign, Positive &&left,
                                    Positive &&right) {
          return std::visit(
              [sign](auto &&a, auto &&b) {
                return sign ? -Rational{a, b} : Rational{a, b};
              },
              left.value, right.value);
        }

        static RealValue real(Base base) {
          return base;
        }

        static RealValue real(Natural natural) {
          return std::visit([](auto &&v) { return RealValue(v); },
                            positive(std::move(natural)));
        }

        static RealValue real(Positive positive) {
          return std::visit([](const auto &value) { return RealValue(value); },
                            positive.value);
        }

        static RealValue real(Negative negative) { return negative; }

        static RealValue real(Integer integer) {
          return std::visit([](const auto &value) { return RealValue(value); },
                            integer.value);
        }

        static RealValue reduce(bool sign, Positive left, Positive right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational(sign, std::move(left), std::move(right));
          }
          if (right == 1u) {

            if (sign) {
              return real(-left);
            }
            return real(left);
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
            return to_rational(sign, std::move(left), std::move(right));
          }
          if (left == aPrime) {
            return to_rational(sign, Base{1u}, floor_div(right, aPrime));
          }
          if (right == aPrime) {
            if (sign) {
              return real(-floor_div(left, aPrime));
            }
            return real(floor_div(left, aPrime));
          }
          return to_rational(sign, floor_div(left, aPrime),
                             floor_div(right, aPrime));
        }

        template <typename Left, typename Right>
        RealValue reduce(const Left &left, const Right &right) {
          if (left == 0u) {
            return Base{0u};
          }
          if (left == right) {
            return Base{1u};
          }
          if (left == 1u || right == 0u) {
            return Rational{left, right};
          }
          if (right == 1u) {
            return real(left);
          }
          return reduce((left < 0) ^ (right < 0), Positive(+left),
                        Positive(+right));
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
        Real::Real(Natural natural) : value(real(std::move(natural))) {}
        Real::Real(Positive positive) : value(real(std::move(positive))) {}
        Real::Real(Negative negative) : value(real(std::move(negative))) {}
        Real::Real(Integer integer) : value(real(std::move(integer))) {}
        Real::Real(Rational rational) : value(real(std::move(rational))) {}
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
