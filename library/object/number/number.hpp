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
#include <numeric>
#include <variant>
#include <vector>

#include <tao/operators.hpp>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Return, typename Op>
        struct Operation {
          template <typename... Args>
          Return operator()(const Args &... args) const {
            return Return(Op{}(args...));
          }
        };

        template <typename Return>
        struct Construct {
          template <typename... Args>
          Return operator()(Args &&... args) const {
            return Return(std::forward<Args>(args)...);
          }
        };

        template <typename Return, typename Left, typename Op>
        struct LeftOperation {
          const Left &left;

          template <typename... Args>
          Return operator()(Args &&... args) const {
            return Return(Op{}(left, std::forward<Args>(args)...));
          }
        };

        template <typename Return, typename Right, typename Op>
        struct RightOperation {
          const Right &right;

          template <typename... Args>
          Return operator()(Args &&... args) const {
            return Return(Op{}(std::forward<Args>(args)..., right));
          }
        };

        struct Identity {
          template <typename Arg>
          constexpr Arg &&operator()(Arg &&arg) const noexcept {
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

          template <typename T>
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

          template <typename T>
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

          template <typename T>
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

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
              return true;
            }
            return std::visit(
                [](const auto &n, const auto &d) { return T(n) / T(d); },
                numerator, denominator);
          }
        };
        using RealValue = std::variant<Base, Natural, Negative, Rational>;
        struct Imag {
          RealValue value;

          template <typename T>
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

          template <typename T>
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
          Number(std::uint64_t i); // NOLINT
          Number(Base base); // NOLINT
          Number(Natural &&natural); // NOLINT
          Number(Negative &&negative); // NOLINT
          Number(Rational &&rational); // NOLINT
          Number(Imag &&imag); // NOLINT
          Number(Complex &&complex); // NOLINT

          Number(const Number &other);
          Number(Number &&other) noexcept;

          ~Number() noexcept;

          Number &operator=(const Number &other);
          Number &operator=(Number &&other) noexcept;

          void swap(Number &&other) noexcept;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit(Construct<T>{}, value);
          }

          Number operator+() const;
          Number operator-() const;
          Number &operator+=(const Number &right);
          Number &operator-=(const Number &right);
          Number &operator*=(const Number &right);
          Number &operator/=(const Number &right);
          Number &operator%=(const Number &right);
          Number operator~() const;
          Number &operator&=(const Number &right);
          Number &operator|=(const Number &right);
          Number &operator^=(const Number &right);
          Number &operator<<=(const Number &right);
          Number &operator>>=(const Number &right);
          bool operator==(const Number &right) const;
          bool operator<(const Number &right) const;

          Number floor_div(const Number &right) const;
          Number gcd(const Number &right) const;

          Number pow(const Number &right) const;
          Number pow(const Number &y, const Number &z) const;

          bool is_int() const;

          bool is_complex() const;

          Number complex() const;

          template <typename OStream>
          OStream &debug(OStream &os) const;

          template <typename OStream>
          OStream &repr(OStream &os) const;

        private:
          template <typename Visitor>
          Number visit(Visitor &&visitor) const;

          template <typename Visitor>
          Number visit(const Number &right, Visitor &&visitor) const;

          NumberValue value;
        };
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
