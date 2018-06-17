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

#define LESS std::visit(std::less<>{}, left.value, right.value)
#define LESS_LEFT                                                              \
  std::visit([&left](const auto &value) { return left < value; }, right.value)
#define LESS_RIGHT                                                             \
  std::visit([&right](const auto &value) { return value < right; }, left.value)
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

        bool operator<(std::uint64_t left, const Positive &right) {
          return LESS_LEFT;
        }

        bool operator<(std::uint64_t /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(std::uint64_t left, const Integer &right) {
          return LESS_LEFT;
        }

        bool operator<(std::uint64_t left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(std::uint64_t left, const Real &right) {
          return LESS_LEFT;
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

        bool operator<(Base left, const Positive &right) {
          return left.value < right;
        }

        bool operator<(Base /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(Base left, const Integer &right) {
          return left.value < right;
        }

        bool operator<(Base left, const Rational &right) {
          return left.value < right;
        }

        bool operator<(Base left, const Real &right) {
          return left.value < right;
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

        bool operator<(const Natural &left, const Positive &right) {
          return LESS_LEFT;
        }

        bool operator<(const Natural & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(const Natural &left, const Integer &right) {
          return LESS_LEFT;
        }

        bool operator<(const Natural &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Natural &left, const Real &right) {
          return LESS_LEFT;
        }

        bool operator<(const Positive &left, std::uint64_t right) {
          return LESS_RIGHT;
        }

        bool operator<(const Positive &left, Base right) {
          return left < right.value;
        }

        bool operator<(const Positive &left, const Natural &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Positive &left, const Positive &right) {
          return LESS;
        }

        bool operator<(const Positive & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator<(const Positive &left, const Integer &right) {
          return LESS;
        }

        bool operator<(const Positive &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Positive &left, const Real &right) { return LESS; }

        bool operator<(const Negative & /*left*/, std::uint64_t /*right*/) {
          return true;
        }

        bool operator<(const Negative & /*left*/, Base /*right*/) {
          return true;
        }

        bool operator<(const Negative & /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(const Negative & /*left*/, const Positive & /*right*/) {
          return true;
        }

        bool operator<(const Negative &left, const Negative &right) {
          return std::visit(std::less<>{}, right.value, left.value);
        }

        bool operator<(const Negative &left, const Integer &right) {
          return LESS_LEFT;
        }

        bool operator<(const Negative &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Negative &left, const Real &right) {
          return LESS_LEFT;
        }

        bool operator<(const Integer &left, std::uint64_t right) {
          return LESS_RIGHT;
        }

        bool operator<(const Integer &left, Base right) {
          return left < right.value;
        }

        bool operator<(const Integer &left, const Natural &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Integer &left, const Positive &right) {
          return LESS;
        }

        bool operator<(const Integer &left, const Negative &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Integer &left, const Integer &right) {
          return LESS;
        }

        bool operator<(const Integer &left, const Rational &right) {
          return LESS_RIGHT_RATIONAL;
        }

        bool operator<(const Integer &left, const Real &right) { return LESS; }

        bool operator<(const Rational &left, std::uint64_t right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, Base right) {
          return left < right.value;
        }

        bool operator<(const Rational &left, const Natural &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Positive &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Negative &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Integer &right) {
          return LESS_LEFT_RATIONAL;
        }

        bool operator<(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD) < (lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        bool operator<(const Rational &left, const Real &right) {
          return LESS_LEFT;
        }

        bool operator<(const Real &left, std::uint64_t right) {
          return LESS_RIGHT;
        }

        bool operator<(const Real &left, Base right) {
          return left < right.value;
        }

        bool operator<(const Real &left, const Natural &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Real &left, const Positive &right) { return LESS; }

        bool operator<(const Real &left, const Negative &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Real &left, const Integer &right) { return LESS; }

        bool operator<(const Real &left, const Rational &right) {
          return LESS_RIGHT;
        }

        bool operator<(const Real &left, const Real &right) { return LESS; }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
