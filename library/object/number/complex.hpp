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

#pragma once

#include <cstdint>
#include <limits>
#include <variant>

#include <tao/operators.hpp>

#include "object/number/base.hpp"
#include "object/number/imag.hpp"
#include "object/number/natural.hpp"
#include "object/number/negative.hpp"
#include "object/number/rational.hpp"

namespace chimera::library::object::number {
  class Complex {
  public:
    Complex(std::uint64_t r, std::uint64_t i);
    template <typename I>
    Complex(std::uint64_t r, I &&i) {}
    template <typename R>
    Complex(R &&r, std::uint64_t i) {}
    template <typename R, typename I>
    Complex(R &&r, I &&i) {}
    Complex(const Complex &other);
    Complex(Complex &&other) noexcept;
    ~Complex() noexcept;
    auto operator=(const Complex &other) -> Complex &;
    auto operator=(Complex &&other) noexcept -> Complex &;
    void swap(Complex &&other) noexcept;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      auto t = std::visit(Construct<T>{}, imag);
      if (t == T()) {
        return std::visit(Construct<T>{}, real);
      }
      return std::numeric_limits<T>::signaling_NaN();
    }
    [[nodiscard]] auto floor_div(const Base &right) const noexcept -> Number;
    [[nodiscard]] auto floor_div(const Complex &right) const noexcept
        -> Number;
    [[nodiscard]] auto floor_div(const Imag &right) const noexcept -> Number;
    [[nodiscard]] auto floor_div(const Natural &right) const noexcept
        -> Number;
    [[nodiscard]] auto floor_div(const Negative &right) const noexcept
        -> Number;
    [[nodiscard]] auto floor_div(const Rational &right) const noexcept
        -> Number;
    [[nodiscard]] auto floor_div(const std::uint64_t right) const noexcept
        -> Number;
    [[nodiscard]] auto gcd(const Base &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const Complex &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const Imag &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const Natural &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const Negative &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const Rational &right) const noexcept -> Number;
    [[nodiscard]] auto gcd(const std::uint64_t right) const noexcept
        -> Number;
    [[nodiscard]] auto pow(const Base &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const Complex &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const Imag &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const Natural &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const Negative &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const Rational &right) const noexcept -> Number;
    [[nodiscard]] auto pow(const std::uint64_t right) const noexcept
        -> Number;
    auto operator-() const noexcept -> Number;
    auto operator-(const Base &right) noexcept -> Number;
    auto operator-(const Complex &right) noexcept -> Number;
    auto operator-(const Imag &right) noexcept -> Number;
    auto operator-(const Natural &right) noexcept -> Number;
    auto operator-(const Negative &right) noexcept -> Number;
    auto operator-(const Rational &right) noexcept -> Number;
    auto operator-(const std::uint64_t right) noexcept -> Number;
    auto operator*(const Base &right) noexcept -> Number;
    auto operator*(const Complex &right) noexcept -> Number;
    auto operator*(const Imag &right) noexcept -> Number;
    auto operator*(const Natural &right) noexcept -> Number;
    auto operator*(const Negative &right) noexcept -> Number;
    auto operator*(const Rational &right) noexcept -> Number;
    auto operator*(const std::uint64_t right) noexcept -> Number;
    auto operator/(const Base &right) noexcept -> Number;
    auto operator/(const Complex &right) noexcept -> Number;
    auto operator/(const Imag &right) noexcept -> Number;
    auto operator/(const Natural &right) noexcept -> Number;
    auto operator/(const Negative &right) noexcept -> Number;
    auto operator/(const Rational &right) noexcept -> Number;
    auto operator/(const std::uint64_t right) noexcept -> Number;
    auto operator&(const Base &right) noexcept -> Number;
    auto operator&(const Complex &right) noexcept -> Number;
    auto operator&(const Imag &right) noexcept -> Number;
    auto operator&(const Natural &right) noexcept -> Number;
    auto operator&(const Negative &right) noexcept -> Number;
    auto operator&(const Rational &right) noexcept -> Number;
    auto operator&(const std::uint64_t right) noexcept -> Number;
    auto operator%(const Base &right) noexcept -> Number;
    auto operator%(const Complex &right) noexcept -> Number;
    auto operator%(const Imag &right) noexcept -> Number;
    auto operator%(const Natural &right) noexcept -> Number;
    auto operator%(const Negative &right) noexcept -> Number;
    auto operator%(const Rational &right) noexcept -> Number;
    auto operator%(const std::uint64_t right) noexcept -> Number;
    auto operator^(const Base &right) noexcept -> Number;
    auto operator^(const Complex &right) noexcept -> Number;
    auto operator^(const Imag &right) noexcept -> Number;
    auto operator^(const Natural &right) noexcept -> Number;
    auto operator^(const Negative &right) noexcept -> Number;
    auto operator^(const Rational &right) noexcept -> Number;
    auto operator^(const std::uint64_t right) noexcept -> Number;
    auto operator+() const noexcept -> Number;
    auto operator+(const Base &right) noexcept -> Number;
    auto operator+(const Complex &right) noexcept -> Number;
    auto operator+(const Imag &right) noexcept -> Number;
    auto operator+(const Natural &right) noexcept -> Number;
    auto operator+(const Negative &right) noexcept -> Number;
    auto operator+(const Rational &right) noexcept -> Number;
    auto operator+(const std::uint64_t right) noexcept -> Number;
    auto operator<(const Base &right) const noexcept -> bool;
    auto operator<(const Complex &right) const noexcept -> bool;
    auto operator<(const Imag &right) const noexcept -> bool;
    auto operator<(const Natural &right) const noexcept -> bool;
    auto operator<(const Negative &right) const noexcept -> bool;
    auto operator<(const Rational &right) const noexcept -> bool;
    auto operator<(const std::uint64_t right) const noexcept -> bool;
    auto operator<<(const Base &right) noexcept -> Number;
    auto operator<<(const Complex &right) noexcept -> Number;
    auto operator<<(const Imag &right) noexcept -> Number;
    auto operator<<(const Natural &right) noexcept -> Number;
    auto operator<<(const Negative &right) noexcept -> Number;
    auto operator<<(const Rational &right) noexcept -> Number;
    auto operator<<(const std::uint64_t right) noexcept -> Number;
    auto operator==(const Complex &right) const noexcept -> bool;
    template <typename T>
    auto operator==(T && /*right*/) const noexcept -> bool {
      return false;
    };
    auto operator>>(const Base &right) noexcept -> Number;
    auto operator>>(const Complex &right) noexcept -> Number;
    auto operator>>(const Imag &right) noexcept -> Number;
    auto operator>>(const Natural &right) noexcept -> Number;
    auto operator>>(const Negative &right) noexcept -> Number;
    auto operator>>(const Rational &right) noexcept -> Number;
    auto operator>>(const std::uint64_t right) noexcept -> Number;
    auto operator|(const Base &right) noexcept -> Number;
    auto operator|(const Complex &right) noexcept -> Number;
    auto operator|(const Imag &right) noexcept -> Number;
    auto operator|(const Natural &right) noexcept -> Number;
    auto operator|(const Negative &right) noexcept -> Number;
    auto operator|(const Rational &right) noexcept -> Number;
    auto operator|(const std::uint64_t right) noexcept -> Number;
    auto operator~() const noexcept -> Number;

  private:
    RealValue real;
    RealValue imag;
  };
} // namespace chimera::library::object::number
