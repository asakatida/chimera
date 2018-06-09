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

#include "object/number/div.hpp"

#include <gsl/gsl>

#include "object/number/mult.hpp"
#include "object/number/negative.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        Rational div(Left &&left, const Rational &right) {
          return std::visit([&left](const auto &rN,
                                    const auto &rD) { return left * rD / rN; },
                            right.numerator, right.denominator);
        }

        template <typename Right>
        Rational div(const Rational &left, Right &&right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN / (lD * right);
              },
              left.numerator, left.denominator);
        }

        Rational operator/(std::uint64_t left, Base right) {
          return {Base{left}, right};
        }

        Rational operator/(std::uint64_t left, const Natural &right) {
          return {Base{left}, right};
        }

        Rational operator/(std::uint64_t left, const Positive &right) {
          return {Base{left}, right};
        }

        Rational operator/(std::uint64_t left, const Negative &right) {
          return {Base{left}, right};
        }

        Rational operator/(std::uint64_t left, const Integer &right) {
          return {Base{left}, right};
        }

        Rational operator/(std::uint64_t left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(std::uint64_t /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(Base left, std::uint64_t right) {
          return {left, Base{right}};
        }

        Rational operator/(Base left, Base right) { return {left, right}; }

        Rational operator/(Base left, const Natural &right) {
          return {left, right};
        }

        Rational operator/(Base left, const Positive &right) {
          return {left, right};
        }

        Rational operator/(Base left, const Negative &right) {
          return {left, right};
        }

        Rational operator/(Base left, const Integer &right) {
          return {left, right};
        }

        Rational operator/(Base left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(Base /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(const Natural &left, std::uint64_t right) {
          return {left, Base{right}};
        }

        Rational operator/(const Natural &left, Base right) {
          return {left, right};
        }

        Rational operator/(const Natural &left, const Natural &right) {
          return {left, right};
        }

        Rational operator/(const Natural &left, const Positive &right) {
          return {left, right};
        }

        Rational operator/(const Natural &left, const Negative &right) {
          return {left, right};
        }

        Rational operator/(const Natural &left, const Integer &right) {
          return {left, right};
        }

        Rational operator/(const Natural &left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(const Natural &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(const Positive &left, std::uint64_t right) {
          return {left, Base{right}};
        }

        Rational operator/(const Positive &left, Base right) {
          return {left, right};
        }

        Rational operator/(const Positive &left, const Natural &right) {
          return {left, right};
        }

        Rational operator/(const Positive &left, const Positive &right) {
          return {left, right};
        }

        Rational operator/(const Positive &left, const Negative &right) {
          return {left, right};
        }

        Rational operator/(const Positive &left, const Integer &right) {
          return {left, right};
        }

        Rational operator/(const Positive &left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(const Positive &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(const Negative &left, std::uint64_t right) {
          return {left, Base{right}};
        }

        Rational operator/(const Negative &left, Base right) {
          return {left, right};
        }

        Rational operator/(const Negative &left, const Natural &right) {
          return {left, right};
        }

        Rational operator/(const Negative &left, const Positive &right) {
          return {left, right};
        }

        Rational operator/(const Negative &left, const Negative &right) {
          return {left, right};
        }

        Rational operator/(const Negative &left, const Integer &right) {
          return {left, right};
        }

        Rational operator/(const Negative &left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(const Negative &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(const Integer &left, std::uint64_t right) {
          return {left, Base{right}};
        }

        Rational operator/(const Integer &left, Base right) {
          return {left, right};
        }

        Rational operator/(const Integer &left, const Natural &right) {
          return {left, right};
        }

        Rational operator/(const Integer &left, const Positive &right) {
          return {left, right};
        }

        Rational operator/(const Integer &left, const Negative &right) {
          return {left, right};
        }

        Rational operator/(const Integer &left, const Integer &right) {
          return {left, right};
        }

        Rational operator/(const Integer &left, const Rational &right) {
          return div(left, right);
        }

        Base operator/(const Integer &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Rational operator/(const Rational &left, std::uint64_t right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, Base right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, const Natural &right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, const Positive &right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, const Negative &right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, const Integer &right) {
          return div(left, right);
        }

        Rational operator/(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD) / (lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
        Base operator/(const Rational &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, Base /*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Positive &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Integer &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }
        Base operator/(const Real &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
