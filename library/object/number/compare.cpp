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

#include "object/number/compare.hpp"

#include <functional>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        bool operator==(std::uint64_t left, Base right) {
          return left == right.value;
        }

        bool operator==(std::uint64_t left, const Natural &right) {
          return right == left;
        }

        bool operator==(std::uint64_t left, const Positive &right) {
          return right == left;
        }

        bool operator==(std::uint64_t left, const Negative &right) {
          return right == left;
        }

        bool operator==(std::uint64_t left, const Integer &right) {
          return right == left;
        }

        bool operator==(std::uint64_t left, const Rational &right) {
          return right == left;
        }

        bool operator==(std::uint64_t left, const Real &right) {
          return right == left;
        }

        bool operator==(std::uint64_t /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(std::uint64_t /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(Base left, std::uint64_t right) {
          return left.value == right;
        }

        bool operator==(Base left, Base right) {
          return left.value == right.value;
        }

        bool operator==(Base left, const Natural &right) {
          return right == left;
        }

        bool operator==(Base left, const Positive &right) {
          return right == left;
        }

        bool operator==(Base left, const Negative &right) {
          return right == left;
        }

        bool operator==(Base left, const Integer &right) {
          return right == left;
        }

        bool operator==(Base left, const Rational &right) {
          return right == left;
        }

        bool operator==(Base left, const Real &right) { return right == left; }

        bool operator==(Base /*left*/, const Imag & /*right*/) { return false; }

        bool operator==(Base /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Natural & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator==(const Natural & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator==(const Natural &left, const Natural &right) {
          if (left.value.size() != right.value.size()) {
            return false;
          }
          return std::equal(left.value.begin(), left.value.end(),
                            right.value.begin());
        }

        bool operator==(const Natural &left, const Positive &right) {
          return right == left;
        }

        bool operator==(const Natural &left, const Negative &right) {
          return right == left;
        }

        bool operator==(const Natural &left, const Integer &right) {
          return right == left;
        }

        bool operator==(const Natural &left, const Rational &right) {
          return right == left;
        }

        bool operator==(const Natural &left, const Real &right) {
          return right == left;
        }

        bool operator==(const Natural & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Natural & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Positive &left, Base right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Positive &left, const Positive &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Positive &left, const Negative &right) {
          return right == left;
        }

        bool operator==(const Positive &left, const Integer &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Positive &left, const Rational &right) {
          return right == left;
        }

        bool operator==(const Positive &left, const Real &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Positive & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Positive & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Negative & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator==(const Negative & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator==(const Negative & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator==(const Negative & /*left*/, const Positive & /*right*/) {
          return false;
        }

        bool operator==(const Negative &left, const Negative &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Negative &left, const Integer &right) {
          return right == left;
        }

        bool operator==(const Negative &left, const Rational &right) {
          return right == left;
        }

        bool operator==(const Negative &left, const Real &right) {
          return right == left;
        }

        bool operator==(const Negative & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Negative & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Integer &left, Base right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Integer &left, const Positive &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Integer &left, const Integer &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Integer &left, const Rational &right) {
          return right == left;
        }

        bool operator==(const Integer &left, const Real &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Integer & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Integer & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, const Positive & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator==(const Rational &left, const Rational &right) {
          return std::visit(std::equal_to<>{}, left.numerator,
                            right.numerator) &&
                 std::visit(std::equal_to<>{}, left.denominator,
                            right.denominator);
        }

        bool operator==(const Rational &left, const Real &right) {
          return right == left;
        }

        bool operator==(const Rational & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Real &left, Base right) {
          return std::visit(
              [right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Real &left, const Positive &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Real &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Real &left, const Integer &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }

        bool operator==(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) { return value == right; },
              left.value);
        }

        bool operator==(const Real &left, const Real &right) {
          return std::visit(std::equal_to<>{}, left.value, right.value);
        }
        bool operator==(const Real & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Real & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, Base /*right*/) { return false; }

        bool operator==(const Imag & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Positive & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Real & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Imag & /*left*/, const Complex & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, Base /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Natural & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Positive & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Negative & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Real & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Imag & /*right*/) {
          return false;
        }

        bool operator==(const Complex & /*left*/, const Complex & /*right*/) {
          return false;
        }

      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
