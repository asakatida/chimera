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

#include "object/number/less.hpp"

#include <functional>

#include "object/number/mult.hpp"
#include "object/number/util.hpp"

#define LESS_LEFT_RATIONAL                                                     \
  std::visit(                                                                  \
      [&right](const auto &lN, const auto &lD) { return lN < (lD * right); },  \
      left.numerator, left.denominator)
#define LESS_RIGHT_RATIONAL                                                    \
  std::visit(                                                                  \
      [&left](const auto &rN, const auto &rD) { return (left * rD) < rN; },    \
      right.numerator, right.denominator)

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        bool operator<(std::uint64_t left, Base right) {
          return left < right.value;
        }

        bool operator<(std::uint64_t /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(std::uint64_t /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(std::uint64_t left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(std::uint64_t /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(std::uint64_t /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(Base left, std::uint64_t right) {
          return left.value < right;
        }

        bool operator<(Base left, Base right) {
          return left.value < right.value;
        }

        bool operator<(Base /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(Base /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(Base left, const Rational &right) {
          return left.value < right;
        }

        bool operator<(Base /*left*/, const Imag & /*right*/) { return false; }

        bool operator<(Base /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(const Natural & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator<(const Natural & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator<(const Natural &left, const Natural &right) {
          if (left.value.size() == right.value.size()) {
            return std::lexicographical_compare(
                left.value.rbegin(), left.value.rend(), right.value.rbegin(),
                right.value.rend());
          }
          return left.value.size() < right.value.size();
        }

        bool operator<(const Natural & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(const Natural &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Natural & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(const Natural & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(const Negative & /*left*/, std::uint64_t /*right*/) {
          return true;
        }

        bool operator<(const Negative & /*left*/, Base /*right*/) {
          return true;
        }

        bool operator<(const Negative & /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(const Negative &left, const Negative &right) {
          return std::visit(std::less<>{}, right.value, left.value);
        }

        bool operator<(const Negative &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Negative & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(const Negative & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(const Rational &left, std::uint64_t right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, Base right) {
          return left < right.value;
        }

        bool operator<(const Rational &left, const Natural &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Negative &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD) < (lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        bool operator<(const Rational & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(const Rational & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, Base /*right*/) { return false; }

        bool operator<(const Imag & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(const Imag & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator<(const Complex & /*left*/, const Complex & /*right*/) {
          return false;
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
