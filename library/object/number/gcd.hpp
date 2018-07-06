// Copyright (c) 2018 Adam Grandquist <grandquista@gmail.com>
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

#include "object/number/details.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Base gcd(std::uint64_t left, Base right);
        Base gcd(std::uint64_t left, const Natural &right);
        Base gcd(std::uint64_t left, const Positive &right);
        Negative gcd(std::uint64_t left, const Negative &right);
        Integer gcd(std::uint64_t left, const Integer &right);
        Integer gcd(std::uint64_t left, const Rational &right);
        Integer gcd(std::uint64_t left, const Real &right);

        Base gcd(std::uint64_t left, const Imag &right);
        Base gcd(std::uint64_t left, const Complex &right);

        Base gcd(Base left, std::uint64_t right);
        Base gcd(Base left, Base right);
        Base gcd(Base left, const Natural &right);
        Base gcd(Base left, const Positive &right);
        Negative gcd(Base left, const Negative &right);
        Integer gcd(Base left, const Integer &right);
        Integer gcd(Base left, const Rational &right);
        Integer gcd(Base left, const Real &right);

        Base gcd(Base left, const Imag &right);
        Base gcd(Base left, const Complex &right);

        Base gcd(const Natural &left, std::uint64_t right);
        Base gcd(const Natural &left, Base right);
        Positive gcd(const Natural &left, const Natural &right);
        Positive gcd(const Natural &left, const Positive &right);
        Negative gcd(const Natural &left, const Negative &right);
        Integer gcd(const Natural &left, const Integer &right);
        Integer gcd(const Natural &left, const Rational &right);
        Integer gcd(const Natural &left, const Real &right);

        Base gcd(const Natural &left, const Imag &right);
        Base gcd(const Natural &left, const Complex &right);

        Positive gcd(const Positive &left, std::uint64_t right);
        Positive gcd(const Positive &left, Base right);
        Positive gcd(const Positive &left, const Natural &right);
        Positive gcd(const Positive &left, const Positive &right);
        Negative gcd(const Positive &left, const Negative &right);
        Integer gcd(const Positive &left, const Integer &right);
        Integer gcd(const Positive &left, const Rational &right);
        Integer gcd(const Positive &left, const Real &right);

        Base gcd(const Positive &left, const Imag &right);
        Base gcd(const Positive &left, const Complex &right);

        Negative gcd(const Negative &left, std::uint64_t right);
        Negative gcd(const Negative &left, Base right);
        Negative gcd(const Negative &left, const Natural &right);
        Negative gcd(const Negative &left, const Positive &right);
        Positive gcd(const Negative &left, const Negative &right);
        Integer gcd(const Negative &left, const Integer &right);
        Integer gcd(const Negative &left, const Rational &right);
        Integer gcd(const Negative &left, const Real &right);

        Base gcd(const Negative &left, const Imag &right);
        Base gcd(const Negative &left, const Complex &right);

        Integer gcd(const Integer &left, std::uint64_t right);
        Integer gcd(const Integer &left, Base right);
        Integer gcd(const Integer &left, const Natural &right);
        Integer gcd(const Integer &left, const Positive &right);
        Integer gcd(const Integer &left, const Negative &right);
        Integer gcd(const Integer &left, const Integer &right);
        Integer gcd(const Integer &left, const Rational &right);
        Integer gcd(const Integer &left, const Real &right);

        Base gcd(const Integer &left, const Imag &right);
        Base gcd(const Integer &left, const Complex &right);

        Integer gcd(const Rational &left, std::uint64_t right);
        Integer gcd(const Rational &left, Base right);
        Integer gcd(const Rational &left, const Natural &right);
        Integer gcd(const Rational &left, const Positive &right);
        Integer gcd(const Rational &left, const Negative &right);
        Integer gcd(const Rational &left, const Integer &right);
        Integer gcd(const Rational &left, const Rational &right);
        Integer gcd(const Rational &left, const Real &right);

        Base gcd(const Rational &left, const Imag &right);
        Base gcd(const Rational &left, const Complex &right);

        Integer gcd(const Real &left, std::uint64_t right);
        Integer gcd(const Real &left, Base right);
        Integer gcd(const Real &left, const Natural &right);
        Integer gcd(const Real &left, const Positive &right);
        Integer gcd(const Real &left, const Negative &right);
        Integer gcd(const Real &left, const Integer &right);
        Integer gcd(const Real &left, const Rational &right);
        Integer gcd(const Real &left, const Real &right);
        Base gcd(const Real &left, const Imag &right);
        Base gcd(const Real &left, const Complex &right);

        Base gcd(const Imag &left, std::uint64_t right);
        Base gcd(const Imag &left, Base right);
        Base gcd(const Imag &left, const Natural &right);
        Base gcd(const Imag &left, const Positive &right);
        Base gcd(const Imag &left, const Negative &right);
        Base gcd(const Imag &left, const Integer &right);
        Base gcd(const Imag &left, const Rational &right);
        Base gcd(const Imag &left, const Real &right);
        Base gcd(const Imag &left, const Imag &right);
        Base gcd(const Imag &left, const Complex &right);

        Base gcd(const Complex &left, std::uint64_t right);
        Base gcd(const Complex &left, Base right);
        Base gcd(const Complex &left, const Natural &right);
        Base gcd(const Complex &left, const Positive &right);
        Base gcd(const Complex &left, const Negative &right);
        Base gcd(const Complex &left, const Integer &right);
        Base gcd(const Complex &left, const Rational &right);
        Base gcd(const Complex &left, const Real &right);
        Base gcd(const Complex &left, const Imag &right);
        Base gcd(const Complex &left, const Complex &right);

      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
