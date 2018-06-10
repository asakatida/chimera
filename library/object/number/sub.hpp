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

#include "object/number/details.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Integer operator-(std::uint64_t left, Base right);
        Negative operator-(std::uint64_t left, const Natural &right);
        Integer operator-(std::uint64_t left, const Positive &right);
        Positive operator-(std::uint64_t left, const Negative &right);
        Integer operator-(std::uint64_t left, const Integer &right);
        Rational operator-(std::uint64_t left, const Rational &right);

        Base operator-(std::uint64_t left, const Real &right);
        Integer operator-(Base left, std::uint64_t right);
        Integer operator-(Base left, Base right);
        Negative operator-(Base left, const Natural &right);
        Integer operator-(Base left, const Positive &right);
        Positive operator-(Base left, const Negative &right);
        Integer operator-(Base left, const Integer &right);
        Rational operator-(Base left, const Rational &right);

        Base operator-(Base left, const Real &right);
        Positive operator-(const Natural &left, std::uint64_t right);
        Positive operator-(const Natural &left, Base right);
        Integer operator-(const Natural &left, const Natural &right);
        Integer operator-(const Natural &left, const Positive &right);
        Natural operator-(const Natural &left, const Negative &right);
        Integer operator-(const Natural &left, const Integer &right);
        Rational operator-(const Natural &left, const Rational &right);

        Base operator-(const Natural &left, const Real &right);
        Integer operator-(const Positive &left, std::uint64_t right);
        Integer operator-(const Positive &left, Base right);
        Integer operator-(const Positive &left, const Natural &right);
        Integer operator-(const Positive &left, const Positive &right);
        Positive operator-(const Positive &left, const Negative &right);
        Integer operator-(const Positive &left, const Integer &right);
        Rational operator-(const Positive &left, const Rational &right);

        Base operator-(const Positive &left, const Real &right);
        Negative operator-(const Negative &left, std::uint64_t right);
        Negative operator-(const Negative &left, Base right);
        Negative operator-(const Negative &left, const Natural &right);
        Negative operator-(const Negative &left, const Positive &right);
        Integer operator-(const Negative &left, const Negative &right);
        Integer operator-(const Negative &left, const Integer &right);
        Rational operator-(const Negative &left, const Rational &right);

        Base operator-(const Negative &left, const Real &right);
        Integer operator-(const Integer &left, std::uint64_t right);
        Integer operator-(const Integer &left, Base right);
        Integer operator-(const Integer &left, const Natural &right);
        Integer operator-(const Integer &left, const Positive &right);
        Integer operator-(const Integer &left, const Negative &right);
        Integer operator-(const Integer &left, const Integer &right);
        Rational operator-(const Integer &left, const Rational &right);

        Base operator-(const Integer &left, const Real &right);
        Rational operator-(const Rational &left, std::uint64_t right);
        Rational operator-(const Rational &left, Base right);
        Rational operator-(const Rational &left, const Natural &right);
        Rational operator-(const Rational &left, const Positive &right);
        Rational operator-(const Rational &left, const Negative &right);
        Rational operator-(const Rational &left, const Integer &right);
        Rational operator-(const Rational &left, const Rational &right);
        Base operator-(const Rational &left, const Real &right);
        Base operator-(const Real &left, std::uint64_t right);
        Base operator-(const Real &left, Base right);
        Base operator-(const Real &left, const Natural &right);
        Base operator-(const Real &left, const Positive &right);
        Base operator-(const Real &left, const Negative &right);
        Base operator-(const Real &left, const Integer &right);
        Base operator-(const Real &left, const Rational &right);
        Base operator-(const Real &left, const Real &right);

      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
