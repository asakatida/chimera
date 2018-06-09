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

#include "object/number/right_shift.hpp"

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/compare.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Base operator>>(std::uint64_t left, Base right) {
          return {left >> right.value};
        }

        Base operator>>(std::uint64_t /*left*/, const Natural & /*right*/) {
          return Base{};
        }

        Base operator>>(std::uint64_t left, const Positive &right) {
          return std::visit([left](const auto &value) { return left >> value; },
                            right.value);
        }

        Positive operator>>(std::uint64_t left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return Positive(left << value); },
              right.value);
        }

        Positive operator>>(std::uint64_t left, const Integer &right) {
          return std::visit(
              [left](const auto &value) { return Positive(left >> value); },
              right.value);
        }

        Base operator>>(std::uint64_t /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(std::uint64_t /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator>>(Base left, std::uint64_t right) {
          return {left.value >> right};
        }

        Base operator>>(Base left, Base right) {
          return {left.value >> right.value};
        }

        Base operator>>(Base /*left*/, const Natural & /*right*/) {
          return Base{};
        }

        Base operator>>(Base left, const Positive &right) {
          return std::visit([left](const auto &value) { return left >> value; },
                            right.value);
        }

        Positive operator>>(Base left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return Positive(left << value); },
              right.value);
        }

        Positive operator>>(Base left, const Integer &right) {
          return std::visit(
              [left](const auto &value) { return Positive(left >> value); },
              right.value);
        }

        Base operator>>(Base /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(Base /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Positive operator>>(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return left;
          }
          if (std::size_t(right / 64) >= left.value.size()) {
            return Base{};
          }
          auto value = left;
          if (std::ptrdiff_t shift = right / 64; shift != 0) {
            value.value.erase(value.value.begin(), value.value.begin() + shift);
          }
          if (std::size_t shift = right % 64; shift != 0) {
            Natural result{};
            Carryover carryover{};
            for (std::uint64_t i : value.value) {
              auto overflow = carryover.overflow;
              carryover = right_shift(i, shift);
              result.value.push_back(carryover.result | overflow);
            }
            return result;
          }
          return value;
        }

        Positive operator>>(const Natural &left, Base right) {
          return left >> right.value;
        }

        Positive operator>>(const Natural & /*left*/,
                            const Natural & /*right*/) {
          Expects(false);
        }

        Positive operator>>(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) { return left >> value; },
              right.value);
        }

        Natural operator>>(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return left << value; },
              right.value);
        }

        Positive operator>>(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left >> value); },
              right.value);
        }

        Base operator>>(const Natural & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Natural &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Positive operator>>(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Positive operator>>(const Positive &left, Base right) {
          return std::visit(
              [right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Positive operator>>(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Positive operator>>(const Positive &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Positive operator>>(const Positive &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Positive operator>>(const Positive &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value >> right); },
              left.value);
        }

        Base operator>>(const Positive & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Positive &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Negative operator>>(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -(value >> right); },
              left.value);
        }

        Negative operator>>(const Negative &left, Base right) {
          return left >> right.value;
        }

        Negative operator>>(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -(value >> right); },
              left.value);
        }

        Negative operator>>(const Negative &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return -(value >> right); },
              left.value);
        }

        Negative operator>>(const Negative &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return -(value >> right); },
              left.value);
        }

        Negative operator>>(const Negative &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return -(value >> right); },
              left.value);
        }

        Base operator>>(const Negative & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Negative &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Integer operator>>(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Integer operator>>(const Integer &left, Base right) {
          return std::visit(
              [right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Integer operator>>(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Integer operator>>(const Integer &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Integer operator>>(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Integer operator>>(const Integer &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value >> right); },
              left.value);
        }

        Base operator>>(const Integer & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Integer &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Rational & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator>>(const Rational &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, Base /*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Positive &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Integer &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }
        Base operator>>(const Real &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
