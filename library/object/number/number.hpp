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

#include <variant>

#include <tao/operators.hpp>

#include "object/number/details.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        using NumberValue =
            std::variant<Base, Natural, Negative, Rational, Imag, Complex>;

        class Number : tao::operators::commutative_bitwise<Number>,
                       tao::operators::modable<Number>,
                       tao::operators::ordered_field<Number>,
                       tao::operators::shiftable<Number>,
                       tao::operators::unit_steppable<Number> {
        public:
          Number(std::uint64_t i); // NOLINT
          explicit Number(Base base);
          explicit Number(Natural &&natural);
          explicit Number(Positive &&positive);
          explicit Number(Negative &&negative);
          explicit Number(Integer &&integer);
          explicit Number(Rational &&rational);
          explicit Number(Real &&real);
          explicit Number(Imag &&imag);
          explicit Number(Complex &&complex);

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
