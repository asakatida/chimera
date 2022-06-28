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
#include <numeric>
#include <variant>
#include <vector>

#include <tao/operators.hpp>

namespace chimera::library::object::number {
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
  struct UnaryPositive {
    template <typename Arg>
    auto operator()(const Arg &arg) const {
      return +arg;
    }
  };
  struct Base {
    std::uint64_t value;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      if constexpr (std::is_same_v<T, bool>) { // NOLINT
        return value != 0u;
      }
      auto max = std::numeric_limits<T>::max();
      if (value >= static_cast<std::uint64_t>(max)) {
        return max;
      }
      return static_cast<T>(value);
    }
  };
  struct Natural {
    std::vector<std::uint64_t> value;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      if constexpr (std::is_same_v<T, bool>) { // NOLINT
        return true;
      }
      auto max = std::numeric_limits<T>::max();
      if (value.size() > 2) {
        return max;
      }
      auto max128 = static_cast<__uint128_t>(max);
      auto a = (__uint128_t(value[1]) << 64u) | value[0];
      if (a >= max128) {
        return max;
      }
      return static_cast<T>(a & max128);
    }
  };
  using PositiveValue = std::variant<Base, Natural>;
  struct Negative {
    PositiveValue value;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      if constexpr (std::is_same_v<T, bool>) { // NOLINT
        return std::visit(Construct<T>{}, value);
      }
      return -std::visit(Construct<T>{}, value);
    }
  };
  using IntegerValue = std::variant<Base, Natural, Negative>;
  struct Rational {
    IntegerValue numerator;
    IntegerValue denominator;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      if constexpr (std::is_same_v<T, bool>) { // NOLINT
        return true;
      }
      return std::visit(
          [](const auto &n, const auto &d) { return T(n) / T(d); }, numerator,
          denominator);
    }
  };
  using RealValue = std::variant<Base, Natural, Negative, Rational>;
  struct Imag {
    RealValue value;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      auto t = std::visit(Construct<T>{}, value);
      if (t == T()) {
        return t;
      }
      return std::numeric_limits<T>::signaling_NaN();
    }
  };
  struct Complex {
    RealValue real;
    RealValue imag;
    template <typename T,
              typename _ = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit operator T() const noexcept {
      auto t = std::visit(Construct<T>{}, imag);
      if (t == T()) {
        return std::visit(Construct<T>{}, real);
      }
      return std::numeric_limits<T>::signaling_NaN();
    }
  };
  using NumberValue =
      std::variant<Base, Natural, Negative, Rational, Imag, Complex>;
  class Number : tao::operators::commutative_bitwise<Number>,
                 tao::operators::modable<Number>,
                 tao::operators::ordered_field<Number>,
                 tao::operators::shiftable<Number>,
                 tao::operators::unit_steppable<Number> {
  public:
    explicit Number(std::uint64_t i);
    explicit Number(Base base);
    explicit Number(Natural &&natural);
    explicit Number(Negative &&negative);
    explicit Number(Rational &&rational);
    explicit Number(Imag &&imag);
    explicit Number(Complex &&complex);
    Number(const Number &other);
    Number(Number &&other) noexcept;
    ~Number() noexcept;
    auto operator=(const Number &other) -> Number &;
    auto operator=(Number &&other) noexcept -> Number &;
    void swap(Number &&other) noexcept;
    template <typename T>
    explicit operator T() const noexcept {
      return std::visit(Construct<T>{}, value);
    }

    auto operator+() const -> Number;
    auto operator-() const -> Number;
    auto operator+=(const Number &right) -> Number &;
    auto operator-=(const Number &right) -> Number &;
    auto operator*=(const Number &right) -> Number &;
    auto operator/=(const Number &right) -> Number &;
    auto operator%=(const Number &right) -> Number &;
    auto operator~() const -> Number;
    auto operator&=(const Number &right) -> Number &;
    auto operator|=(const Number &right) -> Number &;
    auto operator^=(const Number &right) -> Number &;
    auto operator<<=(const Number &right) -> Number &;
    auto operator>>=(const Number &right) -> Number &;
    auto operator==(const Number &right) const -> bool;
    auto operator<(const Number &right) const -> bool;
    [[nodiscard]] auto floor_div(const Number &right) const -> Number;
    [[nodiscard]] auto gcd(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &right) const -> Number;
    [[nodiscard]] auto pow(const Number &y, const Number &z) const -> Number;
    [[nodiscard]] auto is_int() const -> bool;
    [[nodiscard]] auto is_complex() const -> bool;
    [[nodiscard]] auto complex() const -> Number;
    template <typename OStream>
    auto debug(OStream &os) const -> OStream &;
    template <typename OStream>
    auto repr(OStream &os) const -> OStream &;
    [[nodiscard]] auto unpack() const -> NumberValue { return value; }

  private:
    template <typename Visitor>
    auto visit(Visitor &&visitor) const -> Number;
    template <typename Visitor>
    auto visit(const Number &right, Visitor &&visitor) const -> Number;
    NumberValue value;
  };
} // namespace chimera::library::object::number
