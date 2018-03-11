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

#include <gsl/gsl> // for Ensures

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
        Number::Number(std::uint64_t i) : value(Base{i}) {}
        Number::Number(Base base) : value(base) {}
        Number::Number(Natural natural) : value(std::move(natural)) {}
        Number::Number(Integer integer) : value(std::move(integer)) {}
        Number::Number(Rational rational) : value(std::move(rational)) {}

        static bool to_bool(const Base &base) noexcept {
          return base.value != 0;
        }
        static bool to_bool(const Natural & /*natural*/) noexcept {
          return true;
        }
        static bool to_bool(const Integer &integer) noexcept {
          return std::visit([](const auto &value) { return to_bool(value); }, integer.value);
        }
        static bool to_bool(const Rational & /*rational*/) noexcept {
          return true;
        }

        static long to_long(const Base &base) noexcept {
          if (base.value >= std::uint64_t(std::numeric_limits<long>::max())) {
            return std::numeric_limits<long>::max();
          }
          return long(base.value);
        }
        static long to_long(const Natural & /*natural*/) noexcept {
          return std::numeric_limits<long>::max();
        }
        static long to_long(const Integer &integer) noexcept {
          return -std::visit([](const auto &value) { return to_long(value); }, integer.value);
        }
        static long to_long(const Rational &rational) noexcept {
          return std::visit([](const auto &numerator, const auto &denominator) {
            return to_long(numerator) / to_long(denominator);
          }, rational.numerator, rational.denominator);
        }

        static double to_double(const Base &base) noexcept {
          if (base.value >= std::uint64_t(std::numeric_limits<double>::max())) {
            return std::numeric_limits<double>::max();
          }
          return double(base.value);
        }
        static double to_double(const Natural &natural) noexcept {
          if (natural.value.size() > 2) {
            return std::numeric_limits<double>::max();
          }
          __uint128_t a = natural.value[1];
          a = (a << 64) | natural.value[0];
          if (a >= __uint128_t(std::numeric_limits<double>::max())) {
            return std::numeric_limits<double>::max();
          }
          return double(a);
        }
        static double to_double(const Integer &integer) noexcept {
          return -std::visit([](const auto &value) { return to_double(value); }, integer.value);
        }
        static double to_double(const Rational &rational) noexcept {
          return std::visit([](const auto &numerator, const auto &denominator) {
            return to_double(numerator) / to_double(denominator);
          }, rational.numerator, rational.denominator);
        }

        Number::operator bool() const noexcept {
          return visit([](const auto &v) { return to_bool(v); });
        }
        Number::operator long() const noexcept {
          return visit([](const auto &v) { return to_long(v); });
        }
        Number::operator double() const noexcept {
          return visit([](const auto &v) { return to_double(v); });
        }

        Number Number::operator+() const {
          return visit([](auto a) { return +a; });
        }
        Number Number::operator-() const { return visit(std::negate<>{}); }
        Number Number::operator+(const Number &right) const {
          return visit(right, std::plus<>{});
        }
        Number Number::operator-(const Number &right) const {
          return visit(right, std::minus<>{});
        }
        Number Number::operator*(const Number &right) const {
          return visit(right, std::multiplies<>{});
        }
        Number Number::operator/(const Number &right) const {
          return visit(right, std::divides<>{});
        }
        Number Number::operator%(const Number &right) const {
          return visit(right, std::modulus<>{});
        }
        Number Number::operator~() const { return visit(std::bit_not<>{}); }
        Number Number::operator&(const Number &right) const {
          return visit(right, std::bit_and<>{});
        }
        Number Number::operator|(const Number &right) const {
          return visit(right, std::bit_or<>{});
        }
        Number Number::operator^(const Number &right) const {
          return visit(right, std::bit_xor<>{});
        }
        Number Number::operator<<(const Number &right) const {
          return visit(right, [](auto a, auto b) { return a << b; });
        }
        Number Number::operator>>(const Number &right) const {
          return visit(right, [](auto a, auto b) { return a >> b; });
        }
        bool Number::operator==(const Number &right) const {
          return value.index() == right.value.index() &&
                 visit(right, std::equal_to<>{});
        }
        bool Number::operator!=(const Number &right) const {
          return value.index() != right.value.index() ||
                 visit(right, std::not_equal_to<>{});
        }
        bool Number::operator<(const Number &right) const {
          return visit(right, std::less<>{});
        }
        bool Number::operator>(const Number &right) const {
          return visit(right, std::greater<>{});
        }
        bool Number::operator<=(const Number &right) const {
          return visit(right, std::less_equal<>{});
        }
        bool Number::operator>=(const Number &right) const {
          return visit(right, std::greater_equal<>{});
        }

        Number Number::floor_div(const Number &right) const {
          return visit(right,
                       [](auto a, auto b) { return number::floor_div(a, b); });
        }

        Number Number::pow(const Number &right) const { return right; }
        Number Number::pow(const Number &y, const Number & /*z*/) const {
          return y;
        }

        bool Number::is_int() const {
          return std::holds_alternative<Base>(value) ||
                 std::holds_alternative<Natural>(value) ||
                 std::holds_alternative<Integer>(value);
        }

        bool Number::is_complex() const { return false; }

        Number Number::complex() const { return *this; }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
