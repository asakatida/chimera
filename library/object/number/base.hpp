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

#include <gsl/gsl>
#include <tao/operators.hpp>

namespace chimera::library::object::number {
  class Complex;
  class Imag;
  class Natural;
  class Negative;
  class Number;
  class Rational;
  template <typename Return>
  struct Construct {
    template <typename... Args>
    auto operator()(Args &&...args) const -> Return {
      return Return(std::forward<Args>(args)...);
    }
  };
  template <typename Left, typename Op>
  struct LeftOperation {
    const Left &left;
    template <typename... Args>
    auto operator()(Args &&...args) const {
      return Op{}(left, std::forward<Args>(args)...);
    }
  };
  template <typename Right, typename Op>
  struct RightOperation {
    const Right &right;
    template <typename... Args>
    auto operator()(Args &&...args) const {
      return Op{}(std::forward<Args>(args)..., right);
    }
  };
  struct Identity {
    template <typename Arg>
    constexpr auto operator()(Arg &&arg) const noexcept -> Arg && {
      return std::forward<Arg>(arg);
    }
  };
  class Base {
  public:
    Base();
    explicit Base(std::uint64_t i);
    Base(const Base &other);
    Base(Base &&other) noexcept;
    ~Base() noexcept;
    auto operator=(const Base &other) -> Base &;
    auto operator=(Base &&other) noexcept -> Base &;
    void swap(Base &&other) noexcept;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      if constexpr (std::is_same_v<T, bool>) { // NOLINT
        return value != 0u;
      }
      auto max = std::numeric_limits<T>::max();
      if (value >= gsl::narow<std::uint64_t>(max)) {
        return max;
      }
      return gsl::narow<T>(value);
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
    auto operator==(const Base &right) const noexcept -> bool;
    auto operator==(const std::uint64_t right) const noexcept -> bool;
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
    std::uint64_t value;
  };
} // namespace chimera::library::object::number
