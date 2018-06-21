// Copyright (c) 2018 Adam Grandquist <grandquista@gmail.com>
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

#include <type_traits>

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
          return std::visit(Construct<IntegerValue>{},
                            positive(std::move(value)));
        }

        static IntegerValue integer(Positive &&value) {
          return std::visit(Construct<IntegerValue>{}, std::move(value.value));
        }

        static bool even(std::uint64_t i) { return (i & 1u) == 0u; }
        static bool even(Base i) { return even(i.value); }
        static bool even(const Natural &i) { return even(i.value[0]); }
        static bool even(const Positive &i) {
          return std::visit([](const auto &value) { return even(value); },
                            i.value);
        }

        static Rational to_rational(Positive &&left, Positive &&right) {
          return std::visit(Construct<Rational>{}, std::move(left.value),
                            std::move(right.value));
        }

        static RealValue real(Base base) { return base; }

        static RealValue real(Natural &&natural) {
          return std::visit(Construct<RealValue>{},
                            positive(std::move(natural)));
        }

        static RealValue real(Positive &&positive) {
          return std::visit(Construct<RealValue>{}, std::move(positive.value));
        }

        static RealValue real(Integer &&integer) {
          return std::visit(Construct<RealValue>{}, std::move(integer.value));
        }

        static Positive prime(const Positive &left, const Positive &right) {
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
          return aPrime;
        }

        static RealValue reduce(Positive &&left, Positive &&right) {
          while (even(left) && even(right)) {
            left = left >> 1u;
            right = right >> 1u;
          }
          if (left == 1u) {
            return to_rational(std::move(left), std::move(right));
          }
          if (right == 1u) {
            return real(std::move(left));
          }
          auto aPrime = prime(left, right);
          if (aPrime == 1u) {
            return to_rational(std::move(left), std::move(right));
          }
          if (left == aPrime) {
            return to_rational(Positive(Base{1u}), floor_div(right, aPrime));
          }
          if (right == aPrime) {
            return real(floor_div(left, aPrime));
          }
          return to_rational(floor_div(left, aPrime), floor_div(right, aPrime));
        }

        template <typename Left, typename Right>
        struct Reduce {
          RealValue operator()(Left &&left, Right &&right) {
            if (left == 0u) {
              return Base{0u};
            }
            if (left == right) {
              return Base{1u};
            }
            if (left == 1u || right == 0u) {
              return Rational(ReducedValue{}, std::forward<Left>(left),
                              std::forward<Right>(right));
            }
            if (right == 1u) {
              return real(std::forward<Left>(left));
            }
            return reduce(Positive(std::forward<Left>(left)),
                          Positive(std::forward<Right>(right)));
          }
        };

        template <>
        struct Reduce<Negative, Negative> {
          RealValue operator()(Negative && /*left*/, Negative && /*right*/) {
            Ensures(false);
          }
        };
        template <typename Left>
        struct Reduce<Left, Negative> {
          RealValue operator()(Left && /*left*/, Negative && /*right*/) {
            Ensures(false);
          }
        };
        template <typename Right>
        struct Reduce<Negative, Right> {
          RealValue operator()(Negative && /*left*/, Right && /*right*/) {
            Ensures(false);
          }
        };

        struct ReduceVisit {
          template <typename Left, typename Right>
          RealValue operator()(Left &&left, Right &&right) {
            return Reduce<Left, Right>{}(std::forward<Left>(left),
                                         std::forward<Right>(right));
          }
        };

        static RealValue real(Rational &&rational) {
          return std::visit(ReduceVisit{}, std::move(rational.numerator),
                            std::move(rational.denominator));
        }

        Positive::Positive(Base base) : value(base) {}
        Positive::Positive(Natural &&natural)
            : value(positive(std::move(natural))) {}
        Positive::Positive(ReducedValue /*unused*/, Base base) : value(base) {}
        Positive::Positive(ReducedValue /*unused*/, Natural &&natural)
            : value(std::move(natural)) {}

        Negative::Negative(Base base) : value(base) {}
        Negative::Negative(Natural &&natural)
            : value(positive(std::move(natural))) {}
        Negative::Negative(Positive &&positive)
            : value(std::move(positive.value)) {}
        Negative::Negative(ReducedValue /*unused*/, Base base) : value(base) {}
        Negative::Negative(ReducedValue /*unused*/, Natural &&natural)
            : value(std::move(natural)) {}
        Negative::Negative(ReducedValue /*unused*/, Positive &&positive)
            : value(std::move(positive.value)) {}

        Integer::Integer(std::uint64_t i) : value(Base{i}) {}
        Integer::Integer(Base base) : value(base) {}
        Integer::Integer(Natural &&natural)
            : value(integer(std::move(natural))) {}
        Integer::Integer(Positive &&positive)
            : value(integer(std::move(positive))) {}
        Integer::Integer(Negative &&negative) : value(std::move(negative)) {}
        Integer::Integer(ReducedValue /*unused*/, Base base) : value(base) {}
        Integer::Integer(ReducedValue /*unused*/, Natural &&natural)
            : value(std::move(natural)) {}
        Integer::Integer(ReducedValue /*unused*/, Positive &&positive)
            : value(integer(std::move(positive))) {}
        Integer::Integer(ReducedValue /*unused*/, Negative &&negative)
            : value(std::move(negative)) {}

        Real::Real(Base base) : value(base) {}
        Real::Real(Natural &&natural) : value(real(std::move(natural))) {}
        Real::Real(Positive &&positive) : value(real(std::move(positive))) {}
        Real::Real(Negative &&negative) : value(std::move(negative)) {}
        Real::Real(Integer &&integer) : value(real(std::move(integer))) {}
        Real::Real(Rational &&rational) : value(real(std::move(rational))) {}
        Real::Real(ReducedValue /*unused*/, Base base) : value(base) {}
        Real::Real(ReducedValue /*unused*/, Natural &&natural)
            : value(std::move(natural)) {}
        Real::Real(ReducedValue /*unused*/, Positive &&positive)
            : value(real(std::move(positive))) {}
        Real::Real(ReducedValue /*unused*/, Negative &&negative)
            : value(std::move(negative)) {}
        Real::Real(ReducedValue /*unused*/, Integer &&integer)
            : value(real(std::move(integer))) {}
        Real::Real(ReducedValue /*unused*/, Rational &&rational)
            : value(std::move(rational)) {}
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
