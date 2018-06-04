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

#include "object/number/mult.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        bool operator<(std::uint64_t left, const Base right) {
          return left < right.value;
        }

        bool operator<(std::uint64_t /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(std::uint64_t /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator<(std::uint64_t left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) < rN;
              },
              right.numerator, right.denominator);
        }

        bool operator<(const Base left, std::uint64_t right) {
          return left.value < right;
        }

        bool operator<(const Base left, const Base right) {
          return left.value < right.value;
        }

        bool operator<(const Base /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(const Base /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator<(const Base left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) < rN;
              },
              right.numerator, right.denominator);
        }

        bool operator<(const Natural & /*left*/, std::uint64_t /*right*/) {
          return false;
        }

        bool operator<(const Natural & /*left*/, const Base /*right*/) {
          return false;
        }

        bool operator<(const Natural &left, const Natural &right) {
          if (left.value.size() == right.value.size()) {
            return std::lexicographical_compare(
                left.value.begin(), left.value.end(), right.value.begin(),
                right.value.end());
          }
          return left.value.size() < right.value.size();
        }

        bool operator<(const Natural & /*left*/, const Integer & /*right*/) {
          return false;
        }

        bool operator<(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) < rN;
              },
              right.numerator, right.denominator);
        }

        bool operator<(const Integer & /*left*/, std::uint64_t /*right*/) {
          return true;
        }

        bool operator<(const Integer & /*left*/, const Base /*right*/) {
          return true;
        }

        bool operator<(const Integer & /*left*/, const Natural & /*right*/) {
          return true;
        }

        bool operator<(const Integer &left, const Integer &right) {
          return std::visit([](const auto &a, const auto &b) { return b < a; },
                            left.value, right.value);
        }

        bool operator<(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) < rN;
              },
              right.numerator, right.denominator);
        }

        bool operator<(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN < (lD * right);
              },
              left.numerator, left.denominator);
        }

        bool operator<(const Rational &left, const Base right) {
          return left < right.value;
        }

        bool operator<(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN < (lD * right);
              },
              left.numerator, left.denominator);
        }

        bool operator<(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN < (lD * right);
              },
              left.numerator, left.denominator);
        }

        bool operator<(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD) < (lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
