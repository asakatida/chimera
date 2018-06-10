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
#include "object/number/xor.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static NumberValue number(Positive positive) {
          return std::visit(
              [](const auto &value) { return NumberValue(value); },
              positive.value);
        }
        static NumberValue number(Integer integer) {
          return std::visit(
              [](const auto &value) { return NumberValue(value); },
              integer.value);
        }
        static NumberValue number(Real real) {
          return std::visit(
              [](const auto &value) { return NumberValue(value); }, real.value);
        }

        Number::Number(std::uint64_t i) : value(Base{i}) {}
        Number::Number(Base base) : value(base) {}
        Number::Number(Natural natural) : value(std::move(natural)) {}
        Number::Number(Positive positive)
            : value(number(std::move(positive))) {}
        Number::Number(Negative negative) : value(std::move(negative)) {}
        Number::Number(Integer integer) : value(number(std::move(integer))) {}
        Number::Number(Rational rational) : value(std::move(rational)) {}
        Number::Number(Real real) : value(number(std::move(real))) {}

        Number Number::operator+() const {
          return visit([](auto a) { return Number(+a); });
        }
        Number Number::operator-() const {
          return visit([](auto a) { return Number(-a); });
        }
        Number Number::operator+(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a + b);
          });
        }
        Number &Number::operator+=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a + b);
          });
          return *this;
        }
        Number Number::operator-(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a - b);
          });
        }
        Number &Number::operator-=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a - b);
          });
          return *this;
        }
        Number Number::operator*(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a * b);
          });
        }
        Number &Number::operator*=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a * b);
          });
          return *this;
        }
        Number Number::operator/(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a / b);
          });
        }
        Number &Number::operator/=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a / b);
          });
          return *this;
        }
        Number Number::operator%(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a % b);
          });
        }
        Number &Number::operator%=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a % b);
          });
          return *this;
        }
        Number Number::operator~() const {
          return visit([](auto a) { return Number(~a); });
        }
        Number Number::operator&(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a & b);
          });
        }
        Number &Number::operator&=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a & b);
          });
          return *this;
        }
        Number Number::operator|(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a | b);
          });
        }
        Number &Number::operator|=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a | b);
          });
          return *this;
        }
        Number Number::operator^(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a ^ b);
          });
        }
        Number &Number::operator^=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a ^ b);
          });
          return *this;
        }
        Number Number::operator<<(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a << b);
          });
        }
        Number &Number::operator<<=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a << b);
          });
          return *this;
        }
        Number Number::operator>>(const Number &right) const {
          return visit(right, [](const auto &a, const auto &b) {
            return Number(a >> b);
          });
        }
        Number &Number::operator>>=(const Number &right) {
          *this = visit(right, [](const auto &a, const auto &b) {
            return Number(a >> b);
          });
          return *this;
        }
        bool Number::operator==(const Number &right) const {
          return value.index() == right.value.index() &&
                 visit(right, std::equal_to<>{});
        }
        bool Number::operator!=(const Number &right) const {
          return !(*this == right);
        }
        bool Number::operator<(const Number &right) const {
          return visit(right, std::less<>{});
        }
        bool Number::operator>(const Number &right) const {
          return right.visit(*this, std::less<>{});
        }
        bool Number::operator<=(const Number &right) const {
          return !right.visit(*this, std::less<>{});
        }
        bool Number::operator>=(const Number &right) const {
          return !visit(right, std::less<>{});
        }

        Number Number::floor_div(const Number &right) const {
          return visit(right, [](auto a, auto b) {
            return Number(number::floor_div(a, b));
          });
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

        bool Number::is_complex() const { return false; }

        Number Number::complex() const { Expects(false); }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
