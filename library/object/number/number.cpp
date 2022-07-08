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
  Number::Number(std::uint64_t i) : value(Base{i}) {}
  void Number::swap(Number &&other) noexcept {
    using std::swap;
    swap(value, other.value);
  }
  auto Number::operator+() const noexcept -> Number {
    return visit([](const auto &a) { return +a; });
  }
  auto Number::operator-() const noexcept -> Number {
    return visit(std::negate<>{});
  }
  auto Number::operator+=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a + b; }));
    return *this;
  }
  auto Number::operator-=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a - b; }));
    return *this;
  }
  auto Number::operator*=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a * b; }));
    return *this;
  }
  auto Number::operator/=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a / b; }));
    return *this;
  }
  auto Number::operator%=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a % b; }));
    return *this;
  }
  auto Number::operator~() const noexcept -> Number {
    return visit(std::bit_not<>{});
  }
  auto Number::operator&=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a & b; }));
    return *this;
  }
  auto Number::operator|=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a | b; }));
    return *this;
  }
  auto Number::operator^=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a ^ b; }));
    return *this;
  }
  auto Number::operator<<=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a << b; }));
    return *this;
  }
  auto Number::operator>>=(const Number &right) noexcept -> Number {
    swap(visit(right, [](auto a, auto b) { return a >> b; }));
    return *this;
  }
  // auto Number::operator==(const Number &right) const noexcept -> bool {
  //   return value.index() == right.value.index() &&
  //          std::visit(std::equal_to<>{}, value, right.value);
  // }
  auto Number::operator==(const std::uint64_t right) const noexcept -> bool {
    if (const Base *base = std::get_if<Base>(&value); base != nullptr) {
      return (*base) == right;
    }
    return false;
  }
  auto Number::operator<(const Number &right) const noexcept -> bool {
    return std::visit(std::less<>{}, value, right.value);
  }
  auto Number::operator<(const std::uint64_t right) const noexcept -> bool {
    return std::visit([right](auto a) { return a < right; }, value);
  }
  auto Number::floor_div(const Number &right) const noexcept -> Number {
    return visit(right, [](auto a, auto b) { return a.floor_div(b); });
  }
  auto Number::gcd(const Number &right) const noexcept -> Number {
    return visit(right, [](auto a, auto b) { return a.gcd(b); });
  }
  auto Number::pow(const Number &right) const noexcept -> Number {
    return visit(right, [](auto a, auto b) { return a.pow(b); });
  }
  auto Number::is_int() const noexcept -> bool {
    return std::holds_alternative<Base>(value) ||
           std::holds_alternative<Natural>(value) ||
           std::holds_alternative<Negative>(value);
  }
  auto Number::is_complex() const noexcept -> bool {
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
  auto Number::complex() const noexcept -> Number {
    return visit(MakeComplexVisit{});
  }
} // namespace chimera::library::object::number

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
