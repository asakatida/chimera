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

#pragma once

#include <cstdint>
#include <numeric>
#include <variant>
#include <vector>

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

        struct ReducedValue {};

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
        struct Positive {
          PositiveValue value;

          explicit Positive(Base base);
          explicit Positive(Natural &&natural);
          Positive(ReducedValue /*unused*/, Base base);
          Positive(ReducedValue /*unused*/, Natural &&natural);

          Positive(const Positive &other) = default;
          Positive(Positive &&other) noexcept = default;

          ~Positive() noexcept = default;

          Positive &operator=(const Positive &other) = default;
          Positive &operator=(Positive &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit(Construct<T>{}, value);
          }
        };
        struct Negative {
          PositiveValue value;

          explicit Negative(Base base);
          explicit Negative(Natural &&natural);
          explicit Negative(Positive &&positive);
          Negative(ReducedValue /*unused*/, Base base);
          Negative(ReducedValue /*unused*/, Natural &&natural);
          Negative(ReducedValue /*unused*/, Positive &&positive);

          Negative(const Negative &other) = default;
          Negative(Negative &&other) noexcept = default;

          ~Negative() noexcept = default;

          Negative &operator=(const Negative &other) = default;
          Negative &operator=(Negative &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            if constexpr (std::is_same_v<T, bool>) { // NOLINT
              return std::visit(Construct<T>{}, value);
            }
            return -std::visit(Construct<T>{}, value);
          }
        };
        using IntegerValue = std::variant<Base, Natural, Negative>;
        struct Integer {
          IntegerValue value;

          explicit Integer(std::uint64_t i);
          explicit Integer(Base base);
          explicit Integer(Natural &&natural);
          explicit Integer(Positive &&positive);
          explicit Integer(Negative &&negative);
          Integer(ReducedValue /*unused*/, Base base);
          Integer(ReducedValue /*unused*/, Natural &&natural);
          Integer(ReducedValue /*unused*/, Positive &&positive);
          Integer(ReducedValue /*unused*/, Negative &&negative);

          Integer(const Integer &other) = default;
          Integer(Integer &&other) noexcept = default;

          ~Integer() noexcept = default;

          Integer &operator=(const Integer &other) = default;
          Integer &operator=(Integer &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit(Construct<T>{}, value);
          }
        };
        struct Rational {
          IntegerValue numerator;
          IntegerValue denominator;

          template <typename Numerator, typename Denominator>
          Rational(Numerator &&num, Denominator &&den)
              : numerator(Integer(std::forward<Numerator>(num)).value),
                denominator(Integer(std::forward<Denominator>(den)).value) {}
          template <typename Numerator, typename Denominator>
          Rational(ReducedValue /*unused*/, Numerator &&num, Denominator &&den)
              : numerator(Integer(ReducedValue{}, std::forward<Numerator>(num))
                              .value),
                denominator(
                    Integer(ReducedValue{}, std::forward<Denominator>(den))
                        .value) {}

          Rational(const Rational &other) = default;
          Rational(Rational &&other) noexcept = default;

          ~Rational() noexcept = default;

          Rational &operator=(const Rational &other) = default;
          Rational &operator=(Rational &&other) noexcept = default;

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
        struct Real {
          RealValue value;

          explicit Real(Base base);
          explicit Real(Natural &&natural);
          explicit Real(Positive &&positive);
          explicit Real(Negative &&negative);
          explicit Real(Integer &&integer);
          explicit Real(Rational &&rational);
          template <typename... Args>
          explicit Real(Args &&... args)
              : Real(Rational(std::forward<Args>(args)...)) {}
          Real(ReducedValue /*unused*/, Base base);
          Real(ReducedValue /*unused*/, Natural &&natural);
          Real(ReducedValue /*unused*/, Positive &&positive);
          Real(ReducedValue /*unused*/, Negative &&negative);
          Real(ReducedValue /*unused*/, Integer &&integer);
          Real(ReducedValue /*unused*/, Rational &&rational);
          template <typename... Args>
          explicit Real(ReducedValue /*unused*/, Args &&... args)
              : Real(ReducedValue{},
                     Rational(ReducedValue{}, std::forward<Args>(args)...)) {}

          Real(const Real &other) = default;
          Real(Real &&other) noexcept = default;

          ~Real() noexcept = default;

          Real &operator=(const Real &other) = default;
          Real &operator=(Real &&other) noexcept = default;

          template <typename T>
          explicit operator T() const noexcept {
            return std::visit(Construct<T>{}, value);
          }
        };
        struct Imag {
          RealValue value;

          explicit Imag(Base base);
          explicit Imag(Natural &&natural);
          explicit Imag(Positive &&positive);
          explicit Imag(Negative &&negative);
          explicit Imag(Integer &&integer);
          explicit Imag(Rational &&rational);
          explicit Imag(Real &&real);
          Imag(ReducedValue /*unused*/, Base base);
          Imag(ReducedValue /*unused*/, Natural &&natural);
          Imag(ReducedValue /*unused*/, Positive &&positive);
          Imag(ReducedValue /*unused*/, Negative &&negative);
          Imag(ReducedValue /*unused*/, Integer &&integer);
          Imag(ReducedValue /*unused*/, Rational &&rational);
          Imag(ReducedValue /*unused*/, Real &&real);

          Imag(const Imag &other) = default;
          Imag(Imag &&other) noexcept = default;

          ~Imag() noexcept = default;

          Imag &operator=(const Imag &other) = default;
          Imag &operator=(Imag &&other) noexcept = default;

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

          explicit Complex(Base base);
          explicit Complex(Natural &&natural);
          explicit Complex(Positive &&positive);
          explicit Complex(Negative &&negative);
          explicit Complex(Integer &&integer);
          explicit Complex(Rational &&rational);
          explicit Complex(Real &&real);
          explicit Complex(Imag &&imag);
          Complex(ReducedValue /*unused*/, Base base);
          Complex(ReducedValue /*unused*/, Natural &&natural);
          Complex(ReducedValue /*unused*/, Positive &&positive);
          Complex(ReducedValue /*unused*/, Negative &&negative);
          Complex(ReducedValue /*unused*/, Integer &&integer);
          Complex(ReducedValue /*unused*/, Rational &&rational);
          Complex(ReducedValue /*unused*/, Real &&real);
          Complex(ReducedValue /*unused*/, Imag &&imag);

          Complex(const Complex &other) = default;
          Complex(Complex &&other) noexcept = default;

          ~Complex() noexcept = default;

          Complex &operator=(const Complex &other) = default;
          Complex &operator=(Complex &&other) noexcept = default;

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
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
