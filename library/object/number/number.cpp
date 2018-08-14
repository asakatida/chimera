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

#include "object/number/number.hpp"

#include <functional>
#include <limits>

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/and.hpp"
#include "object/number/compare.hpp"
#include "object/number/div.hpp"
#include "object/number/floor_div.hpp"
#include "object/number/gcd.hpp"
#include "object/number/invert.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/less.hpp"
#include "object/number/mod.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/or.hpp"
#include "object/number/positive.hpp"
#include "object/number/right_shift.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"
#include "object/number/xor.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static PositiveValue positive(Natural &&value) {
          while ((!value.value.empty()) && value.value.back() == 0) {
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

        // static bool even(std::uint64_t i) { return (i & 1u) == 0u; }
        // static bool even(Base i) { return even(i.value); }
        // static bool even(const Natural &i) { return even(i.value[0]); }

        template <typename Left, typename Right>
        static Rational to_rational(Left &&left, Right &&right) {
          return Rational{std::forward<Left>(left), std::forward<Right>(right)};
        }

        static RealValue real(Base base) { return base; }

        static RealValue real(Natural &&natural) {
          return std::visit(Construct<RealValue>{}, positive(std::move(natural)));
        }

        template <typename Left, typename Right>
        RealValue reduce(Left &&left, Right &&right) {
          auto aPrime = gcd(left, right);
          if (aPrime == 1u) {
            return to_rational(std::move(left), std::move(right));
          }

          if (left == aPrime) {
            return to_rational(Base{1u}, floor_div(right, aPrime));
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
              return Rational{std::forward<Left>(left), std::forward<Right>(right)};
            }

            if (right == 1u) {
              return real(std::forward<Left>(left));
            }

            return reduce(std::forward<Left>(left), std::forward<Right>(right));
          }
        };

        struct ReduceVisit {
          template <typename Left, typename Right>
          RealValue operator()(Left &&left, Right &&right) {
            return Reduce<Left, Right>{}(std::forward<Left>(left),
                                         std::forward<Right>(right));
          }
        };

        template <>
        struct Reduce<Negative, Negative> {
          RealValue operator()(Negative &&left, Negative &&right) {
            return std::visit(ReduceVisit{}, left.value, right.value);
          }
        };

        template <typename Left>
        struct Reduce<Left, Negative> {
          RealValue operator()(Left &&left, Negative &&right) {
            return std::visit(
                Operation<RealValue, std::negate<>>{},
                std::visit(ReduceVisit{}, std::forward<Left>(left), right.value));
          }
        };

        template <typename Right>
        struct Reduce<Negative, Right> {
          RealValue operator()(Negative &&left, Right &&right) {
            return std::visit(
                Operation<RealValue, std::negate<>>{},
                std::visit(ReduceVisit{}, left.value, std::forward<Right>(right)));
          }
        };

        static RealValue real(Rational &&rational) {
          return std::visit(ReduceVisit{}, std::move(rational.numerator),
                            std::move(rational.denominator));
        }

        template <typename Visitor>
        Number Number::visit(Visitor &&visitor) const {
          return std::visit(std::forward<Visitor>(visitor), value);
        }

        template <typename Visitor>
        Number Number::visit(const Number &right, Visitor &&visitor) const {
          return std::visit(std::forward<Visitor>(visitor), value,
                            right.value);
        }

        template <typename Value>
        NumberValue number(Value &&value) {
          return std::visit(Construct<NumberValue>{},
                            std::forward<Value>(value).value);
        }

        Number::Number(std::uint64_t i) : value(Base{i}) {}

        Number::Number(Base base) : value(base) {}

        Number::Number(Natural &&natural) : value(std::move(natural)) {}

        Number::Number(Negative &&negative) : value(std::move(negative)) {}

        Number::Number(Rational &&rational) : value(std::move(rational)) {}

        Number::Number(Imag &&imag) : value(std::move(imag)) {}

        Number::Number(Complex &&complex) : value(std::move(complex)) {}

        Number::Number(const Number &other) : value(other.value) {}

        Number::Number(Number &&other) noexcept {
          swap(std::move(other));
        }

        Number::~Number() noexcept {
          value = Base{0u};
        }

        Number &Number::operator=(const Number &other) {
          if (this != &other) {
            value = other.value;
          }
          return *this;
        }

        Number &Number::operator=(Number &&other) noexcept {
          swap(std::move(other));
          return *this;
        }

        void Number::swap(Number &&other) noexcept {
          using std::swap;
          swap(value, other.value);
        }

        template <typename Op>
        using NOp = Operation<Number, Op>;

        Number Number::operator+() const { return visit(NOp<UnaryPositive>{}); }

        Number Number::operator-() const { return visit(NOp<std::negate<>>{}); }

        Number &Number::operator+=(const Number &right) {
          *this = visit(right, NOp<std::plus<>>{});
          return *this;
        }

        Number &Number::operator-=(const Number &right) {
          *this = visit(right, NOp<std::minus<>>{});
          return *this;
        }

        Number &Number::operator*=(const Number &right) {
          *this = visit(right, NOp<std::multiplies<>>{});
          return *this;
        }

        Number &Number::operator/=(const Number &right) {
          *this = visit(right, NOp<std::divides<>>{});
          return *this;
        }

        Number &Number::operator%=(const Number &right) {
          *this = visit(right, NOp<std::modulus<>>{});
          return *this;
        }

        Number Number::operator~() const {
          return visit(NOp<std::bit_not<>>{});
        }

        Number &Number::operator&=(const Number &right) {
          *this = visit(right, NOp<std::bit_and<>>{});
          return *this;
        }

        Number &Number::operator|=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a | b);
          });
          return *this;
        }

        Number &Number::operator^=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a ^ b);
          });
          return *this;
        }

        Number &Number::operator<<=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a << b);
          });
          return *this;
        }

        Number &Number::operator>>=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a >> b);
          });
          return *this;
        }

        bool Number::operator==(const Number &right) const {
          return value.index() == right.value.index() && std::visit(std::equal_to<>{}, value, right.value);
        }

        bool Number::operator<(const Number &right) const {
          return std::visit(std::less<>{}, value, right.value);
        }

        Number Number::floor_div(const Number &right) const {
          return visit(right, [](auto a, auto b) {
            return Number(number::floor_div(a, b));
          });
        }

        Number Number::gcd(const Number &right) const {
          return visit(
              right, [](auto a, auto b) { return Number(number::gcd(a, b)); });
        }

        Number Number::pow(const Number &right) const { return right; }

        Number Number::pow(const Number &y, const Number & /*z*/) const {
          return y;
        }

        bool Number::is_int() const {
          return std::holds_alternative<Base>(value) ||
                 std::holds_alternative<Natural>(value) ||
                 std::holds_alternative<Negative>(value);
        }

        bool Number::is_complex() const {
          return std::holds_alternative<Imag>(value) ||
                 std::holds_alternative<Complex>(value);
        }

        Number Number::complex() const {
          return visit([](const auto &a) { return Number(Complex(copy(a))); });
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
