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

#include "object/number/gcd.hpp"

#include <algorithm>

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/add.hpp"
#include "object/number/less.hpp"
#include "object/number/mod.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        Number div(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return gcd(left * rD, rN);
              },
              right.numerator, right.denominator);
        }

        template <typename Right>
        Number div(const Rational &left, const Right &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return gcd(lN, lD * right);
              },
              left.numerator, left.denominator);
        }

        Number gcd(std::uint64_t left, Base right) {
          Number aPrime(left), bPrime(right);
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return aPrime;
        }

        Number gcd(std::uint64_t left, const Natural &right) {
          return gcd(right, left);
        }

        Number gcd(std::uint64_t left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return -gcd(left, value); },
              right.value);
        }

        Number gcd(std::uint64_t left, const Rational &right) {
          return div(left, right);
        }

        Number gcd(std::uint64_t /*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(std::uint64_t /*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(Base left, std::uint64_t right) {
          Expects(right != 0);
          return {left.value / right};
        }

        Number gcd(Base left, Base right) {
          Expects(right.value != 0);
          return {left.value / right.value};
        }

        Number gcd(Base left, const Natural &right) {
          return gcd(left.value, right);
        }

        Number gcd(Base left, const Negative &right) {
          return gcd(left.value, right);
        }

        Number gcd(Base left, const Rational &right) {
          return gcd(left.value, right);
        }

        Number gcd(Base /*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(Base /*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(const Natural &left, std::uint64_t right) {
          Number aPrime(Natural{left}), bPrime(Base{right});
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return aPrime;
        }

        Number gcd(const Natural &left, Base right) {
          return gcd(left, right.value);
        }

        Number gcd(const Natural &left, const Natural &right) {
          // TODO(grandquista)
          // while (even(left) && even(right)) {
          //   left = left >> 1u;
          //   right = right >> 1u;
          // }
          // if (left == 1u) {
          //   return to_rational(std::move(left), std::move(right));
          // }
          // if (right == 1u) {
          //   return real(std::move(left));
          // }
          Number aPrime(Natural{left}), bPrime(Natural{right});
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return aPrime;
        }

        // TODO(grandquista)
        // static Number gcd(const Positive &left, const Positive &right) {
        //   auto aPrime = left, bPrime = right;
        //   std::visit(Repr<std::ostream>{std::cout << "left: "}, left.value)
        //   << '\n'; std::visit(Repr<std::ostream>{std::cout << "right: "},
        //   right.value) << '\n'; while (!(aPrime == bPrime)) {
        //     std::visit(Repr<std::ostream>{std::cout << "aPrime: "},
        //     aPrime.value) << '\n'; std::visit(Repr<std::ostream>{std::cout <<
        //     "bPrime: "}, bPrime.value) << '\n'; if (even(aPrime)) {
        //       aPrime = aPrime >> 1u;
        //     } else if (even(bPrime)) {
        //       bPrime = bPrime >> 1u;
        //     } else if (bPrime < aPrime) {
        //       aPrime = +(aPrime - bPrime) >> 1u;
        //     } else {
        //       bPrime = +(bPrime - aPrime) >> 1u;
        //     }
        //   }
        //   std::visit(Repr<std::ostream>{std::cout << "aPrime: "},
        //   aPrime.value) << '\n'; return aPrime;
        // }

        Number gcd(const Natural &left, const Negative &right) {
          return std::visit([&left](const auto &r) { return -gcd(left, r); },
                            right.value);
        }

        Number gcd(const Natural &left, const Rational &right) {
          return div(left, right);
        }

        Number gcd(const Natural &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(const Natural &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -gcd(value, right); },
              left.value);
        }

        Number gcd(const Negative &left, Base right) {
          return gcd(left, right.value);
        }

        Number gcd(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -gcd(value, right); },
              left.value);
        }

        Number gcd(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return gcd(l, r); },
              left.value, right.value);
        }

        Number gcd(const Negative &left, const Rational &right) {
          return div(left, right);
        }

        Number gcd(const Negative &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(const Negative &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(const Rational &left, std::uint64_t right) {
          return div(left, right);
        }

        Number gcd(const Rational &left, Base right) {
          return gcd(left, right.value);
        }

        Number gcd(const Rational &left, const Natural &right) {
          return div(left, right);
        }

        Number gcd(const Rational &left, const Negative &right) {
          return div(left, right);
        }

        Number gcd(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD).gcd(lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
        Number gcd(const Rational &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(const Rational &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, Base /*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(const Imag &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, Base /*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number gcd(const Complex &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
