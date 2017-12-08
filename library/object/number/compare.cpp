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

#include "object/number/less.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        bool operator==(const std::uint64_t &left, const Base &right) {
          return left == right.value;
        }
        bool operator==(const std::uint64_t & /*left*/,
                        const Natural & /*right*/) {
          return false;
        }
        bool operator==(const std::uint64_t & /*left*/,
                        const Integer & /*right*/) {
          return false;
        }
        bool operator==(const std::uint64_t & /*left*/,
                        const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Base &left, const std::uint64_t &right) {
          return left.value == right;
        }
        bool operator==(const Base &left, const Base &right) {
          return left.value == right.value;
        }
        bool operator==(const Base & /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(const Base & /*left*/, const Integer & /*right*/) {
          return false;
        }
        bool operator==(const Base & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Natural & /*left*/,
                        const std::uint64_t & /*right*/) {
          return false;
        }
        bool operator==(const Natural & /*left*/, const Base & /*right*/) {
          return false;
        }
        bool operator==(const Natural &left, const Natural &right) {
          if (left.value.size() != right.value.size()) {
            return false;
          }
          return std::equal(left.value.begin(), left.value.end(),
                            right.value.begin());
        }
        bool operator==(const Natural & /*left*/, const Integer & /*right*/) {
          return false;
        }
        bool operator==(const Natural & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Integer & /*left*/,
                        const std::uint64_t & /*right*/) {
          return false;
        }
        bool operator==(const Integer & /*left*/, const Base & /*right*/) {
          return false;
        }
        bool operator==(const Integer & /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(const Integer &left, const Integer &right) {
          return std::visit([](const auto &a, const auto &b) { return a == b; },
                            left.value, right.value);
        }
        bool operator==(const Integer & /*left*/, const Rational & /*right*/) {
          return false;
        }

        bool operator==(const Rational & /*left*/,
                        const std::uint64_t & /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Base & /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Natural & /*right*/) {
          return false;
        }
        bool operator==(const Rational & /*left*/, const Integer & /*right*/) {
          return false;
        }
        bool operator==(const Rational &left, const Rational &right) {
          return std::visit([](const auto &lN, const auto &lD, const auto &rN,
                               const auto &rD) { return lN == rN && lD == rD; },
                            left.numerator, left.denominator, right.numerator,
                            right.denominator);
        }

        bool operator!=(const std::uint64_t &left, const Base &right) {
          return left != right.value;
        }
        bool operator!=(const std::uint64_t & /*left*/,
                        const Natural & /*right*/) {
          return true;
        }
        bool operator!=(const std::uint64_t & /*left*/,
                        const Integer & /*right*/) {
          return true;
        }
        bool operator!=(const std::uint64_t & /*left*/,
                        const Rational & /*right*/) {
          return true;
        }

        bool operator!=(const Base &left, const std::uint64_t &right) {
          return left.value != right;
        }
        bool operator!=(const Base &left, const Base &right) {
          return left.value != right.value;
        }
        bool operator!=(const Base & /*left*/, const Natural & /*right*/) {
          return true;
        }
        bool operator!=(const Base & /*left*/, const Integer & /*right*/) {
          return true;
        }
        bool operator!=(const Base & /*left*/, const Rational & /*right*/) {
          return true;
        }

        bool operator!=(const Natural & /*left*/,
                        const std::uint64_t & /*right*/) {
          return true;
        }
        bool operator!=(const Natural & /*left*/, const Base & /*right*/) {
          return true;
        }
        bool operator!=(const Natural &left, const Natural &right) {
          if (left.value.size() != right.value.size()) {
            return true;
          }
          return !std::equal(left.value.begin(), left.value.end(),
                             right.value.begin());
        }
        bool operator!=(const Natural & /*left*/, const Integer & /*right*/) {
          return true;
        }
        bool operator!=(const Natural & /*left*/, const Rational & /*right*/) {
          return true;
        }

        bool operator!=(const Integer & /*left*/,
                        const std::uint64_t & /*right*/) {
          return true;
        }
        bool operator!=(const Integer & /*left*/, const Base & /*right*/) {
          return true;
        }
        bool operator!=(const Integer & /*left*/, const Natural & /*right*/) {
          return true;
        }
        bool operator!=(const Integer &left, const Integer &right) {
          return std::visit([](const auto &a, const auto &b) { return a != b; },
                            left.value, right.value);
        }
        bool operator!=(const Integer & /*left*/, const Rational & /*right*/) {
          return true;
        }

        bool operator!=(const Rational & /*left*/,
                        const std::uint64_t & /*right*/) {
          return true;
        }
        bool operator!=(const Rational & /*left*/, const Base & /*right*/) {
          return true;
        }
        bool operator!=(const Rational & /*left*/, const Natural & /*right*/) {
          return true;
        }
        bool operator!=(const Rational & /*left*/, const Integer & /*right*/) {
          return true;
        }
        bool operator!=(const Rational &left, const Rational &right) {
          return std::visit([](const auto &lN, const auto &lD, const auto &rN,
                               const auto &rD) { return lN != rN || lD != rD; },
                            left.numerator, left.denominator, right.numerator,
                            right.denominator);
        }

        bool operator>(const std::uint64_t &left, const Base &right) {
          return right < left;
        }
        bool operator>(const std::uint64_t &left, const Natural &right) {
          return right < left;
        }
        bool operator>(const std::uint64_t &left, const Integer &right) {
          return right < left;
        }
        bool operator>(const std::uint64_t &left, const Rational &right) {
          return right < left;
        }

        bool operator>(const Base &left, const std::uint64_t &right) {
          return right < left;
        }
        bool operator>(const Base &left, const Base &right) {
          return right < left;
        }
        bool operator>(const Base &left, const Natural &right) {
          return right < left;
        }
        bool operator>(const Base &left, const Integer &right) {
          return right < left;
        }
        bool operator>(const Base &left, const Rational &right) {
          return right < left;
        }

        bool operator>(const Natural &left, const std::uint64_t &right) {
          return right < left;
        }
        bool operator>(const Natural &left, const Base &right) {
          return right < left;
        }
        bool operator>(const Natural &left, const Natural &right) {
          return right < left;
        }
        bool operator>(const Natural &left, const Integer &right) {
          return right < left;
        }
        bool operator>(const Natural &left, const Rational &right) {
          return right < left;
        }

        bool operator>(const Integer &left, const std::uint64_t &right) {
          return right < left;
        }
        bool operator>(const Integer &left, const Base &right) {
          return right < left;
        }
        bool operator>(const Integer &left, const Natural &right) {
          return right < left;
        }
        bool operator>(const Integer &left, const Integer &right) {
          return right < left;
        }
        bool operator>(const Integer &left, const Rational &right) {
          return right < left;
        }

        bool operator>(const Rational &left, const std::uint64_t &right) {
          return right < left;
        }
        bool operator>(const Rational &left, const Base &right) {
          return right < left;
        }
        bool operator>(const Rational &left, const Natural &right) {
          return right < left;
        }
        bool operator>(const Rational &left, const Integer &right) {
          return right < left;
        }
        bool operator>(const Rational &left, const Rational &right) {
          return right < left;
        }

        bool operator>=(const std::uint64_t &left, const Base &right) {
          return !(left < right);
        }
        bool operator>=(const std::uint64_t &left, const Natural &right) {
          return !(left < right);
        }
        bool operator>=(const std::uint64_t &left, const Integer &right) {
          return !(left < right);
        }
        bool operator>=(const std::uint64_t &left, const Rational &right) {
          return !(left < right);
        }

        bool operator>=(const Base &left, const std::uint64_t &right) {
          return !(left < right);
        }
        bool operator>=(const Base &left, const Base &right) {
          return !(left < right);
        }
        bool operator>=(const Base &left, const Natural &right) {
          return !(left < right);
        }
        bool operator>=(const Base &left, const Integer &right) {
          return !(left < right);
        }
        bool operator>=(const Base &left, const Rational &right) {
          return !(left < right);
        }

        bool operator>=(const Natural &left, const std::uint64_t &right) {
          return !(left < right);
        }
        bool operator>=(const Natural &left, const Base &right) {
          return !(left < right);
        }
        bool operator>=(const Natural &left, const Natural &right) {
          return !(left < right);
        }
        bool operator>=(const Natural &left, const Integer &right) {
          return !(left < right);
        }
        bool operator>=(const Natural &left, const Rational &right) {
          return !(left < right);
        }

        bool operator>=(const Integer &left, const std::uint64_t &right) {
          return !(left < right);
        }
        bool operator>=(const Integer &left, const Base &right) {
          return !(left < right);
        }
        bool operator>=(const Integer &left, const Natural &right) {
          return !(left < right);
        }
        bool operator>=(const Integer &left, const Integer &right) {
          return !(left < right);
        }
        bool operator>=(const Integer &left, const Rational &right) {
          return !(left < right);
        }

        bool operator>=(const Rational &left, const std::uint64_t &right) {
          return !(left < right);
        }
        bool operator>=(const Rational &left, const Base &right) {
          return !(left < right);
        }
        bool operator>=(const Rational &left, const Natural &right) {
          return !(left < right);
        }
        bool operator>=(const Rational &left, const Integer &right) {
          return !(left < right);
        }
        bool operator>=(const Rational &left, const Rational &right) {
          return !(left < right);
        }

        bool operator<=(const std::uint64_t &left, const Base &right) {
          return !(right < left);
        }
        bool operator<=(const std::uint64_t &left, const Natural &right) {
          return !(right < left);
        }
        bool operator<=(const std::uint64_t &left, const Integer &right) {
          return !(right < left);
        }
        bool operator<=(const std::uint64_t &left, const Rational &right) {
          return !(right < left);
        }

        bool operator<=(const Base &left, const std::uint64_t &right) {
          return !(right < left);
        }
        bool operator<=(const Base &left, const Base &right) {
          return !(right < left);
        }
        bool operator<=(const Base &left, const Natural &right) {
          return !(right < left);
        }
        bool operator<=(const Base &left, const Integer &right) {
          return !(right < left);
        }
        bool operator<=(const Base &left, const Rational &right) {
          return !(right < left);
        }

        bool operator<=(const Natural &left, const std::uint64_t &right) {
          return !(right < left);
        }
        bool operator<=(const Natural &left, const Base &right) {
          return !(right < left);
        }
        bool operator<=(const Natural &left, const Natural &right) {
          return !(right < left);
        }
        bool operator<=(const Natural &left, const Integer &right) {
          return !(right < left);
        }
        bool operator<=(const Natural &left, const Rational &right) {
          return !(right < left);
        }

        bool operator<=(const Integer &left, const std::uint64_t &right) {
          return !(right < left);
        }
        bool operator<=(const Integer &left, const Base &right) {
          return !(right < left);
        }
        bool operator<=(const Integer &left, const Natural &right) {
          return !(right < left);
        }
        bool operator<=(const Integer &left, const Integer &right) {
          return !(right < left);
        }
        bool operator<=(const Integer &left, const Rational &right) {
          return !(right < left);
        }

        bool operator<=(const Rational &left, const std::uint64_t &right) {
          return !(right < left);
        }
        bool operator<=(const Rational &left, const Base &right) {
          return !(right < left);
        }
        bool operator<=(const Rational &left, const Natural &right) {
          return !(right < left);
        }
        bool operator<=(const Rational &left, const Integer &right) {
          return !(right < left);
        }
        bool operator<=(const Rational &left, const Rational &right) {
          return !(right < left);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
