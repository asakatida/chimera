// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
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
#include "object/number/xor.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

namespace chimera::library::object::number {
  static auto reduce(Base value) -> NumberValue { return value; }
  static auto reduce(Natural &&value) -> NumberValue {
    while ((!value.value.empty()) && value.value.back() == 0) {
      value.value.pop_back();
    }
    if (value.value.empty()) {
      return Base{0U};
    }
    if (value.value.size() == 1) {
      return Base{value.value[0]};
    }
    value.value.shrink_to_fit();
    return value;
  }
  static auto reduce(Negative &&negative) -> NumberValue { return negative; }
  template <typename Left, typename Right>
  auto reduce(Left &&left, Right &&right) -> NumberValue {
    if (left == Base{0U}) {
      return Base{0U};
    }
    if (left == right) {
      return Base{1U};
    }
    if (left == Base{1U} || right == Base{0U}) {
      return Rational{std::forward<Left>(left), std::forward<Right>(right)};
    }
    if (right == Base{1U}) {
      return reduce(std::forward<Left>(left));
    }
    auto aPrime = gcd(left, right);
    if (aPrime == Number(1U)) {
      return Rational{std::forward<Left>(left), std::forward<Right>(right)};
    }
    const Number nLeft(std::forward<Left>(left));
    const Number nRight(std::forward<Right>(right));
    if (nLeft == aPrime) {
      return (Number(1U) / nRight.floor_div(aPrime)).unpack();
    }
    if (nRight == aPrime) {
      return nLeft.floor_div(aPrime).unpack();
    }
    return (nLeft.floor_div(aPrime) / nRight.floor_div(aPrime)).unpack();
  }
  struct Reduce {
    template <typename Left, typename Right>
    auto operator()(Left &&left, Right &&right) -> NumberValue {
      return reduce(std::forward<Left>(left), std::forward<Right>(right));
    }
  };
  static auto reduce(Rational &&rational) -> NumberValue {
    return std::visit(Reduce{}, std::move(rational.numerator),
                      std::move(rational.denominator));
  }
  template <typename Visitor>
  auto Number::visit(Visitor &&visitor) const -> Number {
    return std::visit(std::forward<Visitor>(visitor), value);
  }
  template <typename Visitor>
  auto Number::visit(const Number &right, Visitor &&visitor) const -> Number {
    return std::visit(std::forward<Visitor>(visitor), value, right.value);
  }
  Number::Number(std::uint64_t i) : value(Base{i}) {}
  Number::Number(Base base) : value(base) {}
  Number::Number(Natural natural) : value(reduce(std::move(natural))) {}
  Number::Number(Negative negative) : value(reduce(std::move(negative))) {}
  Number::Number(Rational rational) : value(reduce(std::move(rational))) {}
  Number::Number(Imag imag) : value(std::move(imag)) {}
  Number::Number(Complex complex) : value(std::move(complex)) {}
  void Number::swap(Number &&other) noexcept {
    using std::swap;
    swap(value, other.value);
  }
  auto Number::operator+() const -> Number { return visit(UnaryPositive{}); }
  auto Number::operator-() const -> Number { return visit(std::negate<>{}); }
  auto Number::operator+=(const Number &right) -> Number & {
    *this = visit(right, std::plus<>{});
    return *this;
  }
  auto Number::operator-=(const Number &right) -> Number & {
    *this = visit(right, std::minus<>{});
    return *this;
  }
  auto Number::operator*=(const Number &right) -> Number & {
    *this = visit(right, std::multiplies<>{});
    return *this;
  }
  auto Number::operator/=(const Number &right) -> Number & {
    *this = visit(right, std::divides<>{});
    return *this;
  }
  auto Number::operator%=(const Number &right) -> Number & {
    *this = visit(right, std::modulus<>{});
    return *this;
  }
  auto Number::operator~() const -> Number { return visit(std::bit_not<>{}); }
  auto Number::operator&=(const Number &right) -> Number & {
    *this = visit(right, std::bit_and<>{});
    return *this;
  }
  auto Number::operator|=(const Number &right) -> Number & {
    *this = visit(right, std::bit_or<>{});
    return *this;
  }
  auto Number::operator^=(const Number &right) -> Number & {
    *this = visit(right, [](auto &&a, auto &&b) { return Number(a ^ b); });
    return *this;
  }
  auto Number::operator<<=(const Number &right) -> Number & {
    *this = visit(right, [](auto &&a, auto &&b) { return Number(a << b); });
    return *this;
  }
  auto Number::operator>>=(const Number &right) -> Number & {
    *this = visit(right, [](auto &&a, auto &&b) { return Number(a >> b); });
    return *this;
  }
  auto Number::operator==(const Number &right) const -> bool {
    return value.index() == right.value.index() &&
           std::visit(std::equal_to<>{}, value, right.value);
  }
  auto Number::operator<(const Number &right) const -> bool {
    return std::visit(std::less<>{}, value, right.value);
  }
  auto Number::floor_div(const Number &right) const -> Number {
    return visit(right, [](auto &&a, auto &&b) {
      return Number(number::floor_div(a, b));
    });
  }
  auto Number::gcd(const Number &right) const -> Number {
    return visit(right,
                 [](auto &&a, auto &&b) { return Number(number::gcd(a, b)); });
  }
  auto Number::pow(const Number &right) -> Number { return right; }
  auto Number::pow(const Number &y, const Number & /*z*/) -> Number {
    return y;
  }
  auto Number::is_int() const -> bool {
    return std::holds_alternative<Base>(value) ||
           std::holds_alternative<Natural>(value) ||
           std::holds_alternative<Negative>(value);
  }
  auto Number::is_complex() const -> bool {
    return std::holds_alternative<Imag>(value) ||
           std::holds_alternative<Complex>(value);
  }
  template <typename Arg>
  struct MakeComplex {
    auto operator()(Arg &&arg) const noexcept -> Number {
      return Number(std::decay_t<Arg>(arg));
    }
  };
  struct MakeComplexVisit {
    template <typename Arg>
    auto operator()(Arg &&arg) const noexcept -> Number {
      return MakeComplex<Arg>{}(std::forward<Arg>(arg));
    }
  };
  auto Number::complex() const -> Number { return visit(MakeComplexVisit{}); }
} // namespace chimera::library::object::number

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
